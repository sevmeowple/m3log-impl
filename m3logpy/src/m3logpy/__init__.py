import os
import datetime
import sys

class M3Log:
    """M3Log - 简单易用的日志工具

    基于 m3log 协议标准 v0.1.0
    """
    
    # 日志级别定义
    DEBUG = "DEBUG"
    INFO = "INFO"
    WARN = "WARN"
    ERROR = "ERROR"
    FATAL = "FATAL"
    
    file_path = None
    save_to_file = False

    def __init__(self, file_path=None):
        """初始化 M3Log

        Args:
            file_path (str, optional): 日志文件路径. 如果提供, 日志将保存到文件
        """
        if file_path:
            self.file_path = file_path
            self.save_to_file = True
        else:
            self.file_path = None
            self.save_to_file = False
    
    def _get_timestamp(self):
        """获取ISO 8601格式的时间戳"""
        return datetime.datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ")
    
    def _format_message(self, tags, level, message):
        """格式化日志消息

        Args:
            tags (list): 标签列表
            level (str): 日志级别
            message (str): 日志消息

        Returns:
            str: 格式化后的日志
        """
        # 处理标签
        tags_str = " ".join(tags) if tags else ""
        tags_part = f"[{tags_str}]" if tags_str else ""
        
        # 处理级别
        level_part = f"#{level}" if level else ""
        
        # 处理消息中的换行符
        message = message.replace("\n", "\\n")
        
        # 组装日志行
        timestamp = f"@{self._get_timestamp()}"
        log_line = f"{timestamp} {tags_part} {level_part}: {message}"
        
        return log_line
    
    def log(self, message, tags=None, level=None):
        """记录日志

        Args:
            message (str): 日志消息
            tags (list, optional): 标签列表
            level (str, optional): 日志级别
        """
        log_line = self._format_message(tags, level, message)
        
        # 输出到控制台
        print(log_line)
        
        # 如果需要，保存到文件
        if self.save_to_file and self.file_path:
            try:
                with open(self.file_path, 'a', encoding='utf-8') as f:
                    f.write(log_line + "\n")
            except IOError as e:
                print(f"Error writing to log file: {e}", file=sys.stderr)
    
    # 便捷方法
    def debug(self, message, tags=None):
        """记录调试日志"""
        self.log(message, tags, self.DEBUG)
    
    def info(self, message, tags=None):
        """记录信息日志"""
        self.log(message, tags, self.INFO)
    
    def warn(self, message, tags=None):
        """记录警告日志"""
        self.log(message, tags, self.WARN)
    
    def error(self, message, tags=None):
        """记录错误日志"""
        self.log(message, tags, self.ERROR)
    
    def fatal(self, message, tags=None):
        """记录致命错误日志"""
        self.log(message, tags, self.FATAL)