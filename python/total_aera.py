import os

def calculate_total_resources(file_path):
    total_resources = [0] * 8

    with open(file_path, 'r') as file:
        for line in file:
            parts = line.split()
            if len(parts) == 9:  # 确保每行有9个部分（节点名 + 8种资源）
                resources = list(map(int, parts[1:]))
                for i in range(8):
                    total_resources[i] += resources[i]

    return total_resources

# 文件路径
file_path = './exapmle/case03/design.are'

# 打印绝对路径以进行调试
absolute_path = os.path.abspath(file_path)
print(f"绝对路径: {absolute_path}")

# 计算总资源
total_resources = calculate_total_resources(absolute_path)

# 输出结果
print("八种资源的总量分别是：")
for i, resource in enumerate(total_resources):
    print(f"资源{i+1}: {resource}")