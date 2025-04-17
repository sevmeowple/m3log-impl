/**
 * @file m3log.h
 * @brief m3log C 库头文件，实现 m3log 协议的解析和生成
 * @version 0.1.0
 * 
 * m3log 协议格式:
 * @时间戳 [标签1 标签2 ...] #日志级别: 消息内容
 */

#ifndef M3LOG_H
#define M3LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>  /* 用于 size_t */
#include <time.h>    /* 用于 time_t */

/**
 * 日志级别枚举
 */
typedef enum {
    M3LOG_LEVEL_DEBUG,
    M3LOG_LEVEL_INFO,
    M3LOG_LEVEL_WARN,
    M3LOG_LEVEL_ERROR,
    M3LOG_LEVEL_FATAL,
    M3LOG_LEVEL_UNKNOWN
} m3log_level_t;

/**
 * 包含多个标签的结构
 */
typedef struct {
    char **tags;     /* 标签数组 */
    size_t count;    /* 标签数量 */
} m3log_tags_t;

/**
 * 解析后的日志结构
 */
typedef struct {
    char *time;          /* ISO 8601 格式的时间戳 */
    m3log_tags_t tags;   /* 标签列表 */
    m3log_level_t level; /* 日志级别 */
    char *content;       /* 消息内容 */
} m3log_entry_t;

/**
 * 错误码
 */
typedef enum {
    M3LOG_SUCCESS = 0,
    M3LOG_ERROR_INVALID_FORMAT,
    M3LOG_ERROR_MEMORY_ALLOCATION,
    M3LOG_ERROR_INVALID_ARGUMENT,
    M3LOG_ERROR_BUFFER_TOO_SMALL
} m3log_error_t;

/**
 * 初始化 m3log 库
 * @return M3LOG_SUCCESS 或错误码
 */
m3log_error_t m3log_init(void);

/**
 * 清理 m3log 库资源
 */
void m3log_cleanup(void);

/**
 * 解析 m3log 格式的日志字符串
 * @param log_string 要解析的日志字符串
 * @param entry 用于存储解析结果的结构体指针
 * @return M3LOG_SUCCESS 或错误码
 */
m3log_error_t m3log_parse(const char *log_string, m3log_entry_t *entry);

/**
 * 生成 m3log 格式的日志字符串
 * @param entry 日志条目结构体
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @return 成功时返回写入的字节数，失败时返回负的错误码
 */
int m3log_format(const m3log_entry_t *entry, char *buffer, size_t buffer_size);

/**
 * 创建新的日志条目
 * @param content 日志内容
 * @param level 日志级别
 * @param tags 标签数组
 * @param tag_count 标签数量
 * @return 创建的日志条目，使用后需调用 m3log_free_entry 释放
 */
m3log_entry_t* m3log_create_entry(const char *content, m3log_level_t level, 
                                  const char **tags, size_t tag_count);

/**
 * 释放日志条目结构体中的资源
 * @param entry 要释放的日志条目
 */
void m3log_free_entry(m3log_entry_t *entry);

/**
 * 将日志级别枚举转换为字符串
 * @param level 日志级别
 * @return 日志级别字符串
 */
const char* m3log_level_to_string(m3log_level_t level);

/**
 * 将字符串转换为日志级别枚举
 * @param level_str 日志级别字符串
 * @return 日志级别枚举
 */
m3log_level_t m3log_string_to_level(const char *level_str);

/**
 * 快速记录日志的辅助函数
 * @param level 日志级别
 * @param tags 逗号分隔的标签字符串
 * @param format 格式化字符串
 * @param ... 格式化参数
 * @return 成功返回 字符串，否则返回错误码
 */
m3log_error_t m3log_log(m3log_level_t level, const char *tags, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* M3LOG_H */