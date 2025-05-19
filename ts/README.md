# m3log - 轻量级结构化日志库

m3log 是一个简单易用的日志库，支持多种语言，遵循统一的日志协议格式。本部分介绍 TypeScript 实现。

## 安装

```bash
# 使用 npm
npm install m3log

# 使用 yarn
yarn add m3log

# 使用 bun
bun add m3log
```

## 基本用法

### 快速开始

```typescript
import m3log from 'm3log';

// 简单日志输出
m3log.info("应用启动成功");
// 输出: @2023-04-01T15:30:45Z #INFO: 应用启动成功

// 添加标签
m3log.info("用户登录成功", ["user", "auth"]);
// 输出: @2023-04-01T15:30:45Z [user auth] #INFO: 用户登录成功

// 各种日志级别
m3log.debug("调试信息");
m3log.warn("警告信息");
m3log.error("错误信息");
m3log.fatal("致命错误");
```

### 自定义日志记录器

```typescript
import { M3Log } from 'm3log';

// 创建自定义记录器并设置默认标签
const logger = new M3Log(["api", "server"]);

// 输出日志
logger.info("服务器启动成功");
// 输出: @2023-04-01T15:30:45Z [api server] #INFO: 服务器启动成功

// 添加额外标签
logger.warn("数据库连接延迟", ["db", "latency"]);
// 输出: @2023-04-01T15:30:45Z [api server db latency] #WARN: 数据库连接延迟
```

## 高级用法

### 链式调用

```typescript
import { M3Log } from 'm3log';

new M3Log()
  .setTags(["payment", "gateway"])
  .info("交易处理中")
  .addTags("success")
  .info("交易成功");
// 输出:
// @2023-04-01T15:30:45Z [payment gateway] #INFO: 交易处理中
// @2023-04-01T15:30:45Z [payment gateway success] #INFO: 交易成功
```

### 设置日志输出目标

```typescript
import { M3Log, LogTarget } from 'm3log';

// 输出到控制台(默认)
const consoleLogger = new M3Log(["app"], LogTarget.CONSOLE);

// 只输出到文件 (仅Node.js环境)
const fileLogger = new M3Log(["app"], LogTarget.FILE, {
  filePath: "./logs/application.log"
});

// 同时输出到控制台和文件
const bothLogger = new M3Log(["app"], LogTarget.BOTH);

// 不输出日志(仅返回格式化字符串)
const silentLogger = new M3Log(["app"], LogTarget.NONE);
```

### 文件日志配置

```typescript
import { M3Log, LogTarget } from 'm3log';

const logger = new M3Log(["app"], LogTarget.FILE, {
  filePath: "./logs/app.log",    // 日志文件路径
  append: true,                  // 是否追加模式
  encoding: "utf8",              // 文件编码
  rotateSize: 5 * 1024 * 1024,   // 文件大小限制(5MB)，超过后轮转
  maxFiles: 10                   // 最大保留10个轮转文件
});

// 也可以稍后更改配置
logger.setFileOptions({
  rotateSize: 1024 * 1024,       // 改为1MB轮转
  maxFiles: 3                    // 只保留3个文件
});
```

### 禁用自动时间戳

```typescript
import { M3Log } from 'm3log';

const logger = new M3Log();
logger.setAutoTimestamp(false);
logger.info("无时间戳日志");
// 输出: #INFO: 无时间戳日志
```

### 不带日志级别的原始日志

```typescript
import { M3Log } from 'm3log';

const logger = new M3Log(["system"]);
logger.raw("这是一条原始日志消息");
// 输出: @2023-04-01T15:30:45Z [system]: 这是一条原始日志消息
```

## API 参考

### M3Log 类

- **constructor(tags?: string[], logTarget?: LogTarget, fileOptions?: FileLogOptions)**：创建新的日志实例
- **setAutoTimestamp(auto: boolean)**：设置是否自动添加时间戳
- **addTags(...tags: string[])**：添加新标签
- **setTags(tags: string[])**：设置标签（替换现有标签）
- **setLogTarget(target: LogTarget)**：设置日志输出目标
- **setFileOptions(options: FileLogOptions)**：设置文件日志选项
- **debug(message: string, tags?: string[])**：输出 DEBUG 级别日志
- **info(message: string, tags?: string[])**：输出 INFO 级别日志
- **warn(message: string, tags?: string[])**：输出 WARN 级别日志
- **error(message: string, tags?: string[])**：输出 ERROR 级别日志
- **fatal(message: string, tags?: string[])**：输出 FATAL 级别日志
- **raw(message: string, tags?: string[])**：输出无日志级别的日志

### LogLevel 枚举

- **DEBUG**：调试信息
- **INFO**：一般信息
- **WARN**：警告信息
- **ERROR**：错误信息
- **FATAL**：致命错误

### LogTarget 枚举

- **CONSOLE**：输出到控制台（默认）
- **FILE**：只输出到文件（仅Node.js环境）
- **BOTH**：同时输出到控制台和文件
- **NONE**：不输出日志

### FileLogOptions 接口

- **filePath**：日志文件路径，默认为 `./logs/app.log`
- **append**：是否追加模式，默认为 `true`
- **encoding**：文件编码，默认为 `utf8`
- **rotateSize**：文件大小限制（字节），超过后轮转，默认为 `10MB`
- **maxFiles**：最大保留文件数，默认为 `5`

## 兼容性

- **浏览器环境**：支持所有现代浏览器，但仅限控制台输出
- **Node.js 环境**：全功能支持，包括文件日志和日志轮转

## 注意事项

- 文件日志功能只在 Node.js 环境可用
- 在浏览器环境中，如果指定了

LogTarget.FILE或LogTarget.BOTH，将自动退回到LogTarget.CONSOLE
