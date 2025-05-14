# m3log - TypeScript 实现

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
const logger = new M3Log(["api", "server"],autoConsole: true);

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

- **constructor(tags?: string[])**：创建新的日志实例，可选初始标签
- **setAutoTimestamp(auto: boolean)**：设置是否自动添加时间戳
- **addTags(...tags: string[])**：添加新标签
- **setTags(tags: string[])**：设置标签（替换现有标签）
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

## 兼容性

支持 Node.js 和现代浏览器环境。
