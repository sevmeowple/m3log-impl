# M3Log

> 一个简单、多语言支持的日志标准和实现

M3Log 是一个轻量级、跨语言的日志框架，遵循统一的 m3log 协议标准 v0.1.0，旨在提供简单一致的日志记录方式。

- [M3Log](#m3log)
  - [协议标准](#协议标准)
  - [语言实现](#语言实现)
    - [Python](#python)
      - [安装](#安装)
      - [基本用法](#基本用法)
      - [高级用法](#高级用法)
    - [C++](#c)
      - [安装](#安装-1)
      - [基本用法](#基本用法-1)
      - [高级用法](#高级用法-1)
    - [C](#c-1)
      - [安装](#安装-2)
      - [基本用法](#基本用法-2)
      - [高级用法](#高级用法-2)

## 协议标准

M3Log 遵循简单的文本格式：

```bash
@时间戳 [标签1 标签2 ...] #日志级别: 消息内容
```

例如：

```bash
@2023-04-01T15:30:45Z [user auth login] #INFO: 用户成功登录，ID=12345
```

- **时间戳**：ISO 8601 格式，可选
- **标签**：空格分隔的标识符列表，用于分类和过滤日志
- **日志级别**：支持 DEBUG、INFO、WARN、ERROR、FATAL
- **消息内容**：实际日志信息

## 语言实现

### Python

#### 安装

```bash
pip install m3logpy
```

#### 基本用法

```python
from m3logpy import M3Log

# 仅控制台输出
log = M3Log()
log.info("这是一条信息日志", ["tag1", "tag2"])

# 同时保存到文件
file_log = M3Log("app.log")
file_log.warn("这是一条警告日志", ["app", "warning"])

# 不同日志级别
log.debug("调试信息", ["debug"])
log.info("普通信息", ["info"])
log.warn("警告信息", ["warning"])
log.error("错误信息", ["error"])
log.fatal("致命错误", ["fatal"])

# 自定义日志（无标签）
log.log("自定义消息")
```

#### 高级用法

```python
# 自定义日志级别
log.log("自定义日志消息", ["custom"], "CUSTOM_LEVEL")

# 禁用控制台输出
file_only_log = M3Log("app.log", console_output=False)
```

### C++

#### 安装

将 `m3log.hh` 和`m3log.cc`添加到您的项目中。

#### 基本用法

```cpp
#include "m3log.hh"

int main() {
    // 获取单例实例
    m3log::Logger& logger = m3log::Logger::instance();
    
    // 设置输出文件（可选）
    logger.setOutputFile("app.log");
    
    // 使用不同的日志级别
    logger.debug({"app", "debug"}, "这是一条调试日志");
    logger.info({"app"}, "这是一条信息日志");
    logger.warn({"app", "warning"}, "这是一条警告日志");
    logger.error({"error"}, "这是一条错误日志");
    logger.fatal({"fatal"}, "这是一条致命错误日志");
    
    // 使用单个标签
    logger.info("tag", "使用单个标签的日志");
    
    // 自定义日志
    logger.log(m3log::LogLevel::INFO, {"custom"}, "自定义日志");
    
    return 0;
}
```

#### 高级用法

```cpp
// 禁用控制台输出
logger.setConsoleOutput(false);

// 设置最低日志级别
logger.setMinLogLevel(m3log::LogLevel::WARN);

// 关闭日志文件
logger.closeOutputFile();
```

### C

#### 安装

将 `m3log.h` 和`m3log.c`添加到您的项目中。

#### 基本用法

```c
#include "m3log.h"

int main() {
    // 初始化库
    m3log_init();
    
    // 创建和记录日志
    const char* tags[] = {"app", "init"};
    m3log_entry_t* entry = m3log_create_entry("应用启动成功", M3LOG_LEVEL_INFO, tags, 2);
    
    // 格式化并输出日志
    char buffer[1024];
    m3log_format(entry, buffer, sizeof(buffer));
    printf("%s\n", buffer);
    
    // 释放资源
    m3log_free_entry(entry);
    
    // 使用便捷函数记录日志
    m3log_log(M3LOG_LEVEL_ERROR, "app,error", "发生错误: %s", "连接失败");
    
    // 清理
    m3log_cleanup();
    
    return 0;
}
```

#### 高级用法

```c
// 解析现有日志字符串
m3log_entry_t entry;
m3log_parse("@2023-04-01T15:30:45Z [user auth] #ERROR: 登录失败", &entry);

// 定制日志格式
char custom_format[1024];
m3log_format(&entry, custom_format, sizeof(custom_format));

// 释放资源
free(entry.time);
m3log_free_tags(&entry.tags);
free(entry.content);
```
