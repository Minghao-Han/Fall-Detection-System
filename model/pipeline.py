import os
from PIL import Image
from torchvision import transforms
import torch
from torch.utils.data import Dataset, DataLoader
from mobilenetv3 import mobilenetv3_large, mobilenetv3_small

net_large = mobilenetv3_large()

net_large.load_state_dict(torch.load('/hy-tmp/official-mobilenet/mobilenetv3-large-1cd25616.pth'))
class CustomImageDataset(Dataset):
    def __init__(self, fall_image_folder,not_fall_image_folder, transform=None):
        self.fall_image_folder = fall_image_folder
        self.not_fall_image_folder=not_fall_image_folder
        self.transform = transform
        self.image_paths = []
        self.labels = []
        for filename in os.listdir(fall_image_folder):
            if filename.lower().endswith(('png', 'jpg', 'jpeg', 'bmp', 'gif')):
                self.image_paths.append(os.path.join(fall_image_folder, filename))
                self.labels.append(1)
        for filename in os.listdir(not_fall_image_folder):
            if filename.lower().endswith(('png', 'jpg', 'jpeg', 'bmp', 'gif')):
                self.image_paths.append(os.path.join(not_fall_image_folder, filename))
                self.labels.append(0)  

    def __len__(self):
        return len(self.image_paths)

    def __getitem__(self, idx):
        img_path = self.image_paths[idx]
        image = Image.open(img_path).convert("RGB")

        if self.transform:
            image = self.transform(image)

        label = self.labels[idx]
        return image, label

# 定义图像转换
transform = transforms.Compose([
    transforms.Resize((224, 224)),
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[1, 1, 1]),
])

# 创建数据集和数据加载器
dataset = CustomImageDataset(fall_image_folder='/hy-tmp/datas/images/train/fall',not_fall_image_folder="/hy-tmp/datas/images/train/not_fall", transform=transform)
data_loader = DataLoader(dataset, batch_size=32, shuffle=True)
import torch
import torch.nn as nn
from torchvision.models import mobilenet_v3_large,mobilenet_v3_small

model = mobilenet_v3_small(pretrained=True)
print(f"total params: {sum(p.numel() for p in model.parameters())}")
# print(model)
# 调整最后的分类层以适应二分类任务
model.classifier[3] = nn.Linear(model.classifier[3].in_features, 2)

# 将模型移动到 GPU（如果可用）
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model = model.to(device)


import torch.optim as optim
from torch.utils.data import random_split

# 定义损失函数和优化器
criterion = nn.CrossEntropyLoss()
optimizer = optim.Adam(model.parameters(), lr=1e-4)

# 划分训练集和验证集
train_size = int(0.8 * len(dataset))
val_size = len(dataset) - train_size
train_dataset, val_dataset = random_split(dataset, [train_size, val_size])

train_loader = DataLoader(train_dataset, batch_size=32, shuffle=True)
val_loader = DataLoader(val_dataset, batch_size=32, shuffle=False)

# 训练模型
num_epochs = 15
TP = 0
TN = 0
FP = 0
FN = 0
for epoch in range(num_epochs):
    model.train()
    running_loss = 0.0
    for images, labels in train_loader:
        images, labels = images.to(device), labels.to(device)

        # 前向传播
        outputs = model(images)
        loss = criterion(outputs, labels)

        # 反向传播和优化
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        running_loss += loss.item()

    print(f"Epoch [{epoch + 1}/{num_epochs}], Loss: {running_loss / len(train_loader)}")

    # 验证模型
    model.eval()
    correct = 0
    total = 0
    with torch.no_grad():
        for images, labels in val_loader:
            images, labels = images.to(device), labels.to(device)
            outputs = model(images)
            _, predicted = torch.max(outputs.data, 1)
            total += labels.size(0)
            correct += (predicted == labels).sum().item()
            TP += ((predicted == 1) & (labels == 1)).sum().item()
            TN += ((predicted == 0) & (labels == 0)).sum().item()
            FP += ((predicted == 1) & (labels == 0)).sum().item()
            FN += ((predicted == 0) & (labels == 1)).sum().item()

    print(f"Validation Accuracy: {100 * correct / total}%")
    recall = TP / (TP + FN)
    precision = TP / (TP + FP)
    f1 = 2 * recall * precision / (recall + precision)
    print(f"Recall: {recall}, Precision: {precision}, F1 Score: {f1}")
    
torch.save(model.state_dict(), '/hy-tmp/result/pure-mobilenet.pth')
# model = MBLFnet()

# # 加载保存的模型参数
# model.load_state_dict(torch.load("/hy-tmp/result/pure-mobilenet.pth"))

# 设置模型为评估模式
model.eval()
model = model.to(device)
model = model.eval()
# print("Start inference")
input = torch.randn(1, 3, 224, 224).float().to(device)
output = model(input)

# # 导出为ONNX
torch.onnx.export(
    model, 
    (input), 
    "/hy-tmp/result/mobilenetv3-fall.onnx", 
    export_params=True, 
    opset_version=10,  # 设置ONNX的opset版本为10
    do_constant_folding=True, 
    input_names=['input'], 
    output_names=['output']
#     # dynamic_axes={'input': {0: 'batch_size'}, 'output': {0: 'batch_size'}, 'hidden': {0: 'batch_size'}, 'cell': {0: 'batch_size'}}
)