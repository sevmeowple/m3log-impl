// index.ts
import * as fs from 'fs';
import * as path from 'path';
import * as os from 'os';

export enum LogLevel {
  DEBUG = "DEBUG",
  INFO = "INFO",
  WARN = "WARN",
  ERROR = "ERROR",
  FATAL = "FATAL"
}

// 日志输出目标枚举
export enum LogTarget {
  CONSOLE = "console",
  FILE = "file",
  BOTH = "both",
  NONE = "none"
}

// 文件写入选项
export interface FileLogOptions {
  filePath?: string;      // 日志文件路径
  append?: boolean;       // 是否追加模式
  encoding?: string;      // 文件编码
  rotateSize?: number;    // 文件大小限制（字节），超过后轮转
  maxFiles?: number;      // 最大保留文件数
}

// 默认文件选项
const DEFAULT_FILE_OPTIONS: FileLogOptions = {
  filePath: path.join(process.cwd(), 'logs', 'app.log'),
  append: true,
  encoding: 'utf8',
  rotateSize: 10 * 1024 * 1024, // 10MB
  maxFiles: 5
};

export class M3Log {
  private tags: string[] = [];
  private autoTimestamp: boolean = true;
  private logTarget: LogTarget = LogTarget.CONSOLE;
  private fileOptions: FileLogOptions;
  private isNode: boolean;

  /**
   * 创建一个新的M3Log实例
   * @param tags 默认标签列表
   * @param logTarget 日志输出目标
   * @param fileOptions 文件写入选项
   */
  constructor(tags: string[] = [], logTarget: LogTarget = LogTarget.CONSOLE, fileOptions?: FileLogOptions) {
    this.tags = tags;
    this.logTarget = logTarget;
    this.fileOptions = { ...DEFAULT_FILE_OPTIONS, ...(fileOptions || {}) };
    
    // 检测是否在Node.js环境
    this.isNode = typeof process !== 'undefined' && 
                  process.versions != null && 
                  process.versions.node != null;
    
    // 如果目标包含文件但不在Node环境，警告并退回到控制台
    if ((logTarget === LogTarget.FILE || logTarget === LogTarget.BOTH) && !this.isNode) {
      console.warn('M3Log: 文件日志在当前环境不可用（仅支持Node.js）。已自动切换到控制台输出。');
      this.logTarget = LogTarget.CONSOLE;
    }
    
    // 确保日志目录存在
    this.ensureLogDirectory();
  }

  /**
   * 确保日志目录存在
   */
  private ensureLogDirectory(): void {
    if (this.isNode && (this.logTarget === LogTarget.FILE || this.logTarget === LogTarget.BOTH)) {
      try {
        const logDir = path.dirname(this.fileOptions.filePath!);
        if (!fs.existsSync(logDir)) {
          fs.mkdirSync(logDir, { recursive: true });
        }
      } catch (error) {
        console.error(`M3Log: 创建日志目录失败: ${error}`);
        // 切换到控制台输出
        this.logTarget = LogTarget.CONSOLE;
      }
    }
  }

  /**
   * 设置日志输出目标
   */
  setLogTarget(target: LogTarget): M3Log {
    if ((target === LogTarget.FILE || target === LogTarget.BOTH) && !this.isNode) {
      console.warn('M3Log: 文件日志在当前环境不可用（仅支持Node.js）。已自动切换到控制台输出。');
      this.logTarget = LogTarget.CONSOLE;
    } else {
      this.logTarget = target;
    }
    return this;
  }

  /**
   * 设置文件日志选项
   */
  setFileOptions(options: FileLogOptions): M3Log {
    this.fileOptions = { ...this.fileOptions, ...options };
    this.ensureLogDirectory();
    return this;
  }

  /**
   * 设置是否自动添加时间戳
   */
  setAutoTimestamp(auto: boolean): M3Log {
    this.autoTimestamp = auto;
    return this;
  }

  /**
   * 添加标签
   */
  addTags(...tags: string[]): M3Log {
    this.tags.push(...tags);
    return this;
  }

  /**
   * 设置标签（替换现有标签）
   */
  setTags(tags: string[]): M3Log {
    this.tags = [...tags];
    return this;
  }

  /**
   * 生成ISO 8601格式的时间戳
   */
  private getTimestamp(): string {
    return new Date().toISOString();
  }

  /**
   * 写入文件日志
   */
  private writeToFile(entry: string): void {
    if (!this.isNode) return;
    
    try {
      const logPath = this.fileOptions.filePath!;
      let shouldRotate = false;
      
      // 检查是否需要轮转
      if (this.fileOptions.rotateSize && fs.existsSync(logPath)) {
        const stats = fs.statSync(logPath);
        shouldRotate = stats.size >= this.fileOptions.rotateSize!;
      }
      
      // 如果需要轮转
      if (shouldRotate) {
        this.rotateLogFiles();
      }
      
      // 写入日志
      fs.appendFileSync(
        logPath,
        entry + os.EOL,
        { encoding: this.fileOptions.encoding as BufferEncoding }
      );
    } catch (error) {
      console.error(`M3Log: 写入文件日志失败: ${error}`);
    }
  }

  /**
   * 轮转日志文件
   */
  private rotateLogFiles(): void {
    try {
      const basePath = this.fileOptions.filePath!;
      const maxFiles = this.fileOptions.maxFiles || 5;
      
      // 删除最旧的文件(如果存在)
      const oldestPath = `${basePath}.${maxFiles}`;
      if (fs.existsSync(oldestPath)) {
        fs.unlinkSync(oldestPath);
      }
      
      // 重命名现有的轮转文件
      for (let i = maxFiles - 1; i >= 1; i--) {
        const oldPath = `${basePath}.${i}`;
        const newPath = `${basePath}.${i + 1}`;
        if (fs.existsSync(oldPath)) {
          fs.renameSync(oldPath, newPath);
        }
      }
      
      // 重命名当前日志文件
      if (fs.existsSync(basePath)) {
        fs.renameSync(basePath, `${basePath}.1`);
      }
    } catch (error) {
      console.error(`M3Log: 轮转日志文件失败: ${error}`);
    }
  }

  /**
   * 格式化并输出日志
   */
  private log(level: LogLevel | null, message: string, tags: string[] = []): string {
    // 合并标签
    const allTags = [...this.tags, ...tags];

    // 构建日志条目
    let entry = "";

    // 添加时间戳
    if (this.autoTimestamp) {
      entry += `@${this.getTimestamp()} `;
    }

    // 添加标签
    if (allTags.length > 0) {
      entry += `[${allTags.join(" ")}] `;
    }

    // 添加日志级别
    if (level) {
      entry += `#${level}: `;
    } else {
      entry += ": ";
    }

    // 添加消息
    entry += message;

    // 输出到控制台
    if (this.logTarget === LogTarget.CONSOLE || this.logTarget === LogTarget.BOTH) {
      console.log(entry);
    }
    
    // 输出到文件
    if ((this.logTarget === LogTarget.FILE || this.logTarget === LogTarget.BOTH) && this.isNode) {
      this.writeToFile(entry);
    }
    
    return entry;
  }

  // 便捷日志方法
  debug(message: string, tags: string[] = []): string {
    return this.log(LogLevel.DEBUG, message, tags);
  }

  info(message: string, tags: string[] = []): string {
    return this.log(LogLevel.INFO, message, tags);
  }

  warn(message: string, tags: string[] = []): string {
    return this.log(LogLevel.WARN, message, tags);
  }

  error(message: string, tags: string[] = []): string {
    return this.log(LogLevel.ERROR, message, tags);
  }

  fatal(message: string, tags: string[] = []): string {
    return this.log(LogLevel.FATAL, message, tags);
  }

  // 无日志级别输出
  raw(message: string, tags: string[] = []): string {
    return this.log(null, message, tags);
  }
}

// 导出默认实例，方便快速使用
export default new M3Log();