import re

def delete_arelines(arefilein_path, arefileout_path):
    with open(arefilein_path, 'r') as file:
        lines = file.readlines()
    with open(arefileout_path, 'w') as file:
        line_count = 0  
        for line in lines:
            line_count += 1  
            if line_count < 100001:  # 如果行数小于100000
                file.write(line)  # 写入该行到输出文件

def delete_netlines(netfilein_path, netfileout_path):
    lines_to_write = []  # 创建一个列表来保存要写入的行
    with open(netfilein_path, 'r') as file:
        for line in file:
            lines = line.split()
            num_str=re.findall(r'\d+',lines[0])#保证source node小于g100000
            num=int(num_str[0])
            if(num<100001):
                lines_to_write.append(line)  # 将整行追加到列表中

    lines_to_write_2 = []
    for line in lines_to_write:
            lines = line.split()
            line_nums_str=re.findall(r'\d+',line)
            index=0
            new_content = line
            while( index<len(line_nums_str)):#去掉每行中大于100000的node
                line_num=int(line_nums_str[index])
                if(line_num>100000):
                    remove_content = lines[index]
                    new_content = new_content.replace(remove_content, '')
                index += 1
            lines_to_write_2.append(new_content)
                
    with open(netfileout_path, 'w') as file:
        for line in lines_to_write_2:
            lines = line.split()
            if len(lines)>2:
                file.write(line)

# 文件路径
arefilein_path = './exapmle/case04/design.are'
arefileout_path = './exapmle/case0000/design.are'
netfilein_path = './exapmle/case04/design.net'
netfileout_path = './exapmle/case0000/design.net'

#delete_arelines(arefilein_path, arefileout_path)
delete_netlines(netfilein_path, netfileout_path)