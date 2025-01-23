# 编译器
CC = gcc

# 编译选项
CFLAGS = -Wall -g

# 链接选项
LDFLAGS = #-lcrypt -ldl

# 目标程序
TARGET = dl_cos

# 源文件
SRCS = dl_cos.c

# 目标文件
OBJS = $(SRCS:.c=.o)

# 默认目标
all: $(TARGET)

# 生成目标程序
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

# 生成目标文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
	rm -f $(OBJS) $(TARGET)
