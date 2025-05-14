// index.ts
export enum LogLevel {
    DEBUG = "DEBUG",
    INFO = "INFO",
    WARN = "WARN",
    ERROR = "ERROR",
    FATAL = "FATAL"
  }
  
  export class M3Log {
    private tags: string[] = [];
    private autoTimestamp: boolean = true;
    private autoConsole:boolean = true;
  
    /**
     * 创建一个新的M3Log实例
     * @param tags 默认标签列表
     */
    constructor(tags: string[] = [],autoConsole = false) {
      this.tags = tags;
      this.autoConsole = autoConsole
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
      
      // console.log(entry);
      return entry;
    }
  
    // 便捷日志方法
    debug(message: string, tags: string[] = []): string {
      console.log(this.log(LogLevel.DEBUG, message, tags));
      return this.log(LogLevel.DEBUG, message, tags);
    }
    
    info(message: string, tags: string[] = []): string {
      console.log(this.log(LogLevel.INFO, message, tags));
      return this.log(LogLevel.INFO, message, tags);
    }
    
    warn(message: string, tags: string[] = []): string {
      console.log(this.log(LogLevel.WARN, message, tags));
      return this.log(LogLevel.WARN, message, tags);
    }
    
    error(message: string, tags: string[] = []): string {
      console.error(this.log(LogLevel.ERROR, message, tags));
      return this.log(LogLevel.ERROR, message, tags);
    }
    
    fatal(message: string, tags: string[] = []): string {
      console.error(this.log(LogLevel.FATAL, message, tags));
      return this.log(LogLevel.FATAL, message, tags);
    }
    
    // 无日志级别输出
    raw(message: string, tags: string[] = []): string {
      console.log(this.log(null, message, tags));
      return this.log(null, message, tags);
    }
  }
  
  // 导出默认实例，方便快速使用
export default new M3Log();