#encoding=utf-8
__author__ = 'anboqing'
import numpy as np

def readClaFile(filename):
    with open(filename,'r') as fp:
        fp.readline()
        nums =  fp.readline().split()
        class_num = int(nums[0])
        model_num = int(nums[1])
        model_indices = dict()
        N = dict()

        for i in range(class_num):
            fp.readline()
            info = fp.readline().split()
            class_name = info[0]
            model_num_class=int(info[2])
            N[class_name] = model_num_class
            for j in range(model_num_class):
                model_index = int(fp.readline())
                model_indices[model_index] = class_name
        return N,model_indices

def modelName2Id(model_indice):
    values = model_indice.values()
    values_set = set(values)
    idx_dict = dict()
    idx = 0
    for class_name in values_set:
        idx_dict[class_name] = idx
        idx+=1
    new_dic = dict()
    for model_name,class_name in model_indice.iteritems():
        new_dic[model_name] = idx_dict[class_name]
    return new_dic

def getModelDict(cla_file_path):
    N,model_indice = readClaFile(cla_file_path)
    model_dic = modelName2Id(model_indice)
    return model_dic

def genFileList(cla_file_path,model_root_dir,dest_path):
    """
    生成模型文件名 <---> classId列表
    """
    model_dic = getModelDict(cla_file_path)
    import commands as cmd
    files_str = cmd.getoutput('ls '+model_root_dir)
    file_names = files_str.split('\n')
    file_list_dic = dict()
    for model_file_name in file_names:
        # get model id from name ie m123->123
        model_id = int(model_file_name[1:-4])
        full_path = model_root_dir+'/'+model_file_name
        file_list_dic[full_path] = model_dic[model_id]

    with open(dest_path,'w') as of:
        for k,v in file_list_dic.iteritems():
            of.write(k)
            of.write(' ')
            of.write(str(v))
            of.write('\n')

def get_model_id(file_name):
    '''
    file_name : 必须是 xxx/xxx/m123
    '''
    idx = file_name.rfind('/')
    id = file_name[idx+2:]
    return int(id)


def genSketchFileList(cla_file_path,sketch_root_path,dest_path):
    import os
    model_dic = getModelDict(cla_file_path)
    file_list_dic = dict()
    list_paths = os.walk(sketch_root_path)
    for root,dirs,files in list_paths:
        for sketch_name in files:
            model_id = get_model_id(root)
            full_path = os.path.join(root,sketch_name)
            file_list_dic[full_path] = model_dic[model_id]

    with open(dest_path,'w') as of:
        for k,v in file_list_dic.iteritems():
            of.write(k)
            of.write(' ')
            of.write(str(v))
            of.write('\n')


if __name__=='__main__':
    data_home = '/home/anboqing/Code/sbe/data/13/'
    cla_file = data_home+'model.cla'
    model_root = data_home+'debug/'
    sketch_root = data_home+'debug/proj'
    #genFileList(cla_file,model_root,'./model_file_list')
    genSketchFileList(cla_file,sketch_root,data_home+'/debug/sketch_list')



