#!/bin/bash

# 脚本名称
SCRIPT_NAME=$(basename "$0")

# 帮助信息
usage() {
    echo "用法: $SCRIPT_NAME [源文件]"
    echo "示例: $SCRIPT_NAME check_pass.c"
    exit 1
}

# 检查参数
if [ $# -ne 1 ]; then
    usage
fi

# 获取源文件名
SRC_FILE="$1"
if [ ! -f "$SRC_FILE" ]; then
    echo "错误: 文件 '$SRC_FILE' 不存在"
    exit 1
fi

# 提取文件名（不带扩展名）
TARGET="${SRC_FILE%.*}"

# 生成 Makefile
generate_makefile() {
    cat <<EOF > Makefile
# 编译器
CC = gcc

# 编译选项
CFLAGS = -Wall -g

# 链接选项
LDFLAGS = -lcrypt -ldl

# 目标程序
TARGET = $TARGET

# 源文件
SRCS = $SRC_FILE

# 目标文件
OBJS = \$(SRCS:.c=.o)

# 默认目标
all: \$(TARGET)

# 生成目标程序
\$(TARGET): \$(OBJS)
	\$(CC) \$(OBJS) \$(LDFLAGS) -o \$(TARGET)

# 生成目标文件
%.o: %.c
	\$(CC) \$(CFLAGS) -c \$< -o \$@

# 清理
clean:
	rm -f \$(OBJS) \$(TARGET)
EOF
}

# 生成 Makefile
generate_makefile
if [ $? -ne 0 ]; then
    echo "错误: 无法生成 Makefile"
    exit 1
fi

# 编译程序
echo "正在编译 $SRC_FILE ..."
make
if [ $? -ne 0 ]; then
    echo "错误: 编译失败"
    exit 1
fi

# 检查生成的可执行文件
if [ -f "$TARGET" ]; then
    echo "编译成功！可执行文件: $TARGET"
else
    echo "错误: 未生成可执行文件"
    exit 1
fi

# # 清理
# make clean
# if [ $? -ne 0 ]; then
#     echo "警告: 清理失败"
# fi

exit 0