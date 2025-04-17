/**
 * @file m3log.c
 * @brief m3log C 库实现
 */

#include "../include/m3log.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
#define strdup _strdup
#endif

/* 内部函数声明 */
static char *m3log_generate_timestamp(void);
static m3log_tags_t m3log_parse_tags(const char *tags_str);
static void m3log_free_tags(m3log_tags_t *tags);
static char *m3log_trim(char *str);

/* 全局初始化标志 */
static int g_m3log_initialized = 0;

m3log_error_t m3log_init(void) {
    if (g_m3log_initialized) {
        return M3LOG_SUCCESS;
    }

    /* 可以在这里进行任何全局初始化 */
    g_m3log_initialized = 1;
    return M3LOG_SUCCESS;
}

void m3log_cleanup(void) {
    if (!g_m3log_initialized) {
        return;
    }

    /* 可以在这里清理全局资源 */
    g_m3log_initialized = 0;
}

m3log_error_t m3log_parse(const char *log_string, m3log_entry_t *entry) {
    if (!log_string || !entry) {
        return M3LOG_ERROR_INVALID_ARGUMENT;
    }

    /* 初始化结果结构体 */
    memset(entry, 0, sizeof(m3log_entry_t));

    char *log_copy = strdup(log_string);
    if (!log_copy) {
        return M3LOG_ERROR_MEMORY_ALLOCATION;
    }

    char *time_part = NULL;
    char *tags_part = NULL;
    char *level_part = NULL;
    char *content_part = NULL;

    /* 提取时间戳部分 (@2023-04-01T15:30:45Z) */
    if (log_copy[0] == '@') {
        time_part = log_copy + 1;
        char *time_end = strchr(time_part, ' ');
        if (time_end) {
            *time_end = '\0';
            entry->time = strdup(time_part);
            time_part = time_end + 1;
        } else {
            free(log_copy);
            return M3LOG_ERROR_INVALID_FORMAT;
        }
    } else {
        time_part = log_copy;
        entry->time = NULL; /* 没有时间戳 */
    }

    /* 提取标签部分 ([标签1 标签2 ...]) */
    tags_part = strchr(time_part, '[');
    if (tags_part) {
        char *tags_end = strchr(tags_part, ']');
        if (tags_end) {
            *tags_end = '\0';
            entry->tags = m3log_parse_tags(tags_part + 1);
            tags_part = tags_end + 1;
        } else {
            free(log_copy);
            if (entry->time)
                free(entry->time);
            return M3LOG_ERROR_INVALID_FORMAT;
        }
    } else {
        /* 没有标签部分 */
        entry->tags.tags = NULL;
        entry->tags.count = 0;
        tags_part = time_part;
    }

    /* 提取级别部分 (#INFO) */
    level_part = strchr(tags_part, '#');
    if (level_part) {
        char *level_end = strchr(level_part, ':');
        if (level_end) {
            *level_end = '\0';
            char *level_str = m3log_trim(level_part + 1);
            entry->level = m3log_string_to_level(level_str);
            content_part = level_end + 1;
        } else {
            free(log_copy);
            if (entry->time)
                free(entry->time);
            m3log_free_tags(&entry->tags);
            return M3LOG_ERROR_INVALID_FORMAT;
        }
    } else {
        /* 没有级别部分 */
        entry->level = M3LOG_LEVEL_UNKNOWN;
        content_part = tags_part;

        /* 如果没有级别，则检查冒号直接分隔内容 */
        char *content_sep = strchr(content_part, ':');
        if (content_sep) {
            content_part = content_sep + 1;
        }
    }

    /* 提取内容部分 */
    if (content_part) {
        entry->content = strdup(m3log_trim(content_part));
    } else {
        entry->content = strdup("");
    }

    free(log_copy);
    return M3LOG_SUCCESS;
}

int m3log_format(const m3log_entry_t *entry, char *buffer, size_t buffer_size) {
    if (!entry || !buffer || buffer_size == 0) {
        return -M3LOG_ERROR_INVALID_ARGUMENT;
    }

    int written = 0;

    /* 添加时间戳 */
    if (entry->time && strlen(entry->time) > 0) {
        written += snprintf(buffer + written, buffer_size - written, "@%s ", entry->time);
    } else {
        char *timestamp = m3log_generate_timestamp();
        written += snprintf(buffer + written, buffer_size - written, "@%s ", timestamp);
        free(timestamp);
    }

    /* 检查缓冲区溢出 */
    if (written >= (int)buffer_size) {
        return -M3LOG_ERROR_BUFFER_TOO_SMALL;
    }

    /* 添加标签列表 */
    written += snprintf(buffer + written, buffer_size - written, "[");
    for (size_t i = 0; i < entry->tags.count; i++) {
        written += snprintf(buffer + written,
                            buffer_size - written,
                            "%s%s",
                            entry->tags.tags[i],
                            (i < entry->tags.count - 1) ? " " : "");

        /* 检查缓冲区溢出 */
        if (written >= (int)buffer_size) {
            return -M3LOG_ERROR_BUFFER_TOO_SMALL;
        }
    }
    written += snprintf(buffer + written, buffer_size - written, "] ");

    /* 检查缓冲区溢出 */
    if (written >= (int)buffer_size) {
        return -M3LOG_ERROR_BUFFER_TOO_SMALL;
    }

    /* 添加日志级别 */
    if (entry->level != M3LOG_LEVEL_UNKNOWN) {
        written += snprintf(buffer + written, buffer_size - written, "#%s: ", m3log_level_to_string(entry->level));
    } else {
        written += snprintf(buffer + written, buffer_size - written, ": ");
    }

    /* 检查缓冲区溢出 */
    if (written >= (int)buffer_size) {
        return -M3LOG_ERROR_BUFFER_TOO_SMALL;
    }

    /* 添加日志内容 */
    if (entry->content) {
        written += snprintf(buffer + written, buffer_size - written, "%s", entry->content);
    }

    /* 确保字符串终止 */
    if (written >= (int)buffer_size) {
        buffer[buffer_size - 1] = '\0';
        return -M3LOG_ERROR_BUFFER_TOO_SMALL;
    }

    return written;
}

m3log_entry_t *m3log_create_entry(const char *content, m3log_level_t level, const char **tags, size_t tag_count) {
    if (!content) {
        return NULL;
    }

    m3log_entry_t *entry = (m3log_entry_t *)malloc(sizeof(m3log_entry_t));
    if (!entry) {
        return NULL;
    }

    /* 初始化结构体 */
    memset(entry, 0, sizeof(m3log_entry_t));

    /* 设置时间戳 */
    entry->time = m3log_generate_timestamp();

    /* 设置日志级别 */
    entry->level = level;

    /* 设置内容 */
    entry->content = strdup(content);

    /* 设置标签 */
    if (tags && tag_count > 0) {
        entry->tags.count = tag_count;
        entry->tags.tags = (char **)malloc(sizeof(char *) * tag_count);

        if (!entry->tags.tags) {
            free(entry->time);
            free(entry->content);
            free(entry);
            return NULL;
        }

        for (size_t i = 0; i < tag_count; i++) {
            entry->tags.tags[i] = tags[i] ? strdup(tags[i]) : strdup("");
        }
    } else {
        entry->tags.count = 0;
        entry->tags.tags = NULL;
    }

    return entry;
}

void m3log_free_entry(m3log_entry_t *entry) {
    if (!entry) {
        return;
    }

    if (entry->time) {
        free(entry->time);
    }

    m3log_free_tags(&entry->tags);

    if (entry->content) {
        free(entry->content);
    }

    free(entry);
}

const char *m3log_level_to_string(m3log_level_t level) {
    switch (level) {
        case M3LOG_LEVEL_DEBUG:
            return "DEBUG";
        case M3LOG_LEVEL_INFO:
            return "INFO";
        case M3LOG_LEVEL_WARN:
            return "WARN";
        case M3LOG_LEVEL_ERROR:
            return "ERROR";
        case M3LOG_LEVEL_FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

m3log_level_t m3log_string_to_level(const char *level_str) {
    if (!level_str) {
        return M3LOG_LEVEL_UNKNOWN;
    }

    if (strcmp(level_str, "DEBUG") == 0)
        return M3LOG_LEVEL_DEBUG;
    if (strcmp(level_str, "INFO") == 0)
        return M3LOG_LEVEL_INFO;
    if (strcmp(level_str, "WARN") == 0)
        return M3LOG_LEVEL_WARN;
    if (strcmp(level_str, "ERROR") == 0)
        return M3LOG_LEVEL_ERROR;
    if (strcmp(level_str, "FATAL") == 0)
        return M3LOG_LEVEL_FATAL;

    return M3LOG_LEVEL_UNKNOWN;
}

m3log_error_t m3log_log(m3log_level_t level, const char *tags, const char *format, ...) {
    if (!format) {
        return M3LOG_ERROR_INVALID_ARGUMENT;
    }

    /* 处理可变参数，格式化消息 */
    va_list args;
    va_start(args, format);

    char content[4096] = {0}; /* 足够大的缓冲区 */
    vsnprintf(content, sizeof(content), format, args);

    va_end(args);

    /* 解析标签 */
    char **tag_array = NULL;
    size_t tag_count = 0;

    if (tags && *tags) {
        /* 计算标签数量 */
        const char *p = tags;
        tag_count = 1; /* 至少有一个标签 */

        while (*p) {
            if (*p == ',') {
                tag_count++;
            }
            p++;
        }

        /* 分配标签数组 */
        tag_array = (char **)malloc(sizeof(char *) * tag_count);
        if (!tag_array) {
            return M3LOG_ERROR_MEMORY_ALLOCATION;
        }

        /* 解析标签 */
        char tags_copy[1024] = {0};
        strncpy(tags_copy, tags, sizeof(tags_copy) - 1);

        char *token = strtok(tags_copy, ",");
        size_t i = 0;

        while (token && i < tag_count) {
            tag_array[i++] = m3log_trim(token);
            token = strtok(NULL, ",");
        }

        tag_count = i; /* 更新实际标签数量 */
    }

    /* 创建日志条目 */
    m3log_entry_t *entry = m3log_create_entry(content, level, (const char **)tag_array, tag_count);

    /* 释放标签数组 */
    free(tag_array);

    if (!entry) {
        return M3LOG_ERROR_MEMORY_ALLOCATION;
    }

    /* 格式化日志并输出 */
    char log_buffer[4096] = {0};
    int result = m3log_format(entry, log_buffer, sizeof(log_buffer));

    m3log_free_entry(entry);

    if (result < 0) {
        return (m3log_error_t)(-result);
    }

    /* 输出日志 (在实际应用中，您可能想要将其写入文件或发送到日志系统) */
    //   printf("%s\n", log_buffer);

    return log_buffer;
}

/* 内部辅助函数实现 */

static char *m3log_generate_timestamp(void) {
    time_t now;
    struct tm *tm_info;
    char timestamp[32] = {0};

    time(&now);

#ifdef _WIN32
    struct tm tm_info_local;
    gmtime_s(&tm_info_local, &now);
    tm_info = &tm_info_local;
#else
    tm_info = gmtime(&now);
#endif

    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", tm_info);

    return strdup(timestamp);
}

static m3log_tags_t m3log_parse_tags(const char *tags_str) {
    m3log_tags_t result = {NULL, 0};

    if (!tags_str || !*tags_str) {
        return result;
    }

    /* 计算标签数量 */
    size_t count = 1;
    const char *p = tags_str;

    while (*p) {
        if (isspace((unsigned char)*p)) {
            count++;
            /* 跳过连续的空格 */
            while (*(p + 1) && isspace((unsigned char)*(p + 1))) {
                p++;
            }
        }
        p++;
    }

    /* 分配标签数组 */
    char **tags = (char **)malloc(sizeof(char *) * count);
    if (!tags) {
        return result;
    }

    /* 解析标签 */
    char *tags_copy = strdup(tags_str);
    if (!tags_copy) {
        free(tags);
        return result;
    }

    size_t i = 0;
    char *token = strtok(tags_copy, " \t\n\r\f\v");

    while (token && i < count) {
        tags[i++] = strdup(token);
        token = strtok(NULL, " \t\n\r\f\v");
    }

    free(tags_copy);

    /* 设置结果 */
    result.tags = tags;
    result.count = i; /* 实际解析的标签数量 */

    return result;
}

static void m3log_free_tags(m3log_tags_t *tags) {
    if (!tags) {
        return;
    }

    if (tags->tags) {
        for (size_t i = 0; i < tags->count; i++) {
            if (tags->tags[i]) {
                free(tags->tags[i]);
            }
        }

        free(tags->tags);
        tags->tags = NULL;
    }

    tags->count = 0;
}

static char *m3log_trim(char *str) {
    if (!str) {
        return NULL;
    }

    /* 跳过前导空格 */
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    /* 如果字符串全是空格 */
    if (!*start) {
        *str = '\0';
        return str;
    }

    /* 查找字符串末尾 */
    char *end = start + strlen(start) - 1;

    /* 修剪尾随空格 */
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    /* 添加终止符 */
    *(end + 1) = '\0';

    /* 如果原字符串前面有空格，则移动字符串 */
    if (start != str) {
        memmove(str, start, (end - start) + 2);
    }

    return str;
}