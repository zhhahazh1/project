from collections import deque, defaultdict

class Node:
    def __init__(self, name):
        self.name = name
        self.neighbors = []

    def add_neighbor(self, neighbor):
        self.neighbors.append(neighbor)

    def get_neighbors(self):
        return self.neighbors

def bfs(start_node, total_nodes):
    q = deque([start_node])
    distance = {start_node: 0}  # 节点到起始节点的距离
    re_distance = defaultdict(deque)
    re_distance[0].append(start_node)

    visited_nodes = 1  # 初始化计数
    maxdis = 0

    while visited_nodes < total_nodes and q:
        current = q.popleft()
        for neighbor in current.get_neighbors():
            if neighbor not in distance:
                maxdis = distance[current] + 1
                distance[neighbor] = maxdis
                re_distance[maxdis].append(neighbor)  # 将节点添加到对应距离的列表中
                q.append(neighbor)
                visited_nodes += 1  # 增加已访问节点计数
                if visited_nodes >= total_nodes:
                    break  # 如果已访问节点数量达到最大节点数量，停止遍历

    # 打印 distance 容器中的元素个数
    print(f"Distance map contains {len(distance)} elements.")
    return distance, maxdis

def read_graph_from_file(filename):
    nodes = {}
    with open(filename, 'r') as file:
        for line in file:
            parts = line.strip().split()
            node_name = parts[0]
            if node_name not in nodes:
                nodes[node_name] = Node(node_name)
            for neighbor_name in parts[2:]:
                if neighbor_name not in nodes:
                    nodes[neighbor_name] = Node(neighbor_name)
                nodes[node_name].add_neighbor(nodes[neighbor_name])
    return nodes

# 从文件中读取图
filename = './exapmle/case03/design.net'
nodes = read_graph_from_file(filename)

# 选择一个起始节点并运行 BFS
start_node = nodes['g9741']
total_nodes = len(nodes)
distance, maxdis = bfs(start_node, total_nodes)

# 打印最大距离
print(f"Maximum distance: {maxdis}")