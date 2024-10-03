def calculate_total_edge_weight(file_path):
    total_weight = 0

    with open(file_path, 'r') as file:
        for line in file:
            parts = line.split()
            if len(parts) > 1:  # 确保行有足够的部分
                weight = int(parts[1])  # 权重是行的第二个值
                total_weight += weight

    return total_weight

# 文件路径
file_path = './exapmle/case03/design.net'

# 计算总权重
total_weight = calculate_total_edge_weight(file_path)

# 输出结果
print(f"边的总权重是: {total_weight}")