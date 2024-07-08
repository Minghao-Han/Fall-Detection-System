/*
 Navicat Premium Data Transfer

 Source Server         : localhost_3306
 Source Server Type    : MySQL
 Source Server Version : 80031 (8.0.31)
 Source Host           : localhost:3306
 Source Schema         : springboot

 Target Server Type    : MySQL
 Target Server Version : 80031 (8.0.31)
 File Encoding         : 65001

 Date: 06/07/2024 15:52:05
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for admin
-- ----------------------------
DROP TABLE IF EXISTS `admin`;
CREATE TABLE `admin`  (
  `id` int NOT NULL AUTO_INCREMENT COMMENT '主键ID',
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '姓名',
  `password` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '密码',
  `sex` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '性别',
  `age` int NULL DEFAULT NULL COMMENT '年龄',
  `phone` varchar(11) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '电话',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `phone`(`phone` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 4 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci COMMENT = '用户信息表' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of admin
-- ----------------------------
INSERT INTO `admin` VALUES (1, '张三', '123456', '男', 25, '15527685070');
INSERT INTO `admin` VALUES (2, '李四', '123456', '女', 25, '18815541851');
INSERT INTO `admin` VALUES (3, '宋剑', '123456', NULL, NULL, NULL);

-- ----------------------------
-- Table structure for camera
-- ----------------------------
DROP TABLE IF EXISTS `camera`;
CREATE TABLE `camera`  (
  `id` int NOT NULL AUTO_INCREMENT COMMENT '主键ID',
  `cameraName` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '摄像机的名字',
  `serialNumber` varchar(16) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '摄像机的序列号',
  `phone` varchar(11) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '绑定用户电话',
  `dateTime` datetime NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '绑定的日期',
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '老人的名字',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `serialnumber`(`serialNumber` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 8 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of camera
-- ----------------------------
INSERT INTO `camera` VALUES (1, '卧室11', '5JF6Q7GGSE0WDLBK', '15527685070', '2024-07-05 00:00:00', '宋剑');
INSERT INTO `camera` VALUES (2, '客厅', 'X4dR7pL3H9mN2bQ1', '18815541851', '2024-07-05 00:00:00', '王立琪');
INSERT INTO `camera` VALUES (3, '卧室2', 'G7kL9mN2pQ8rT4uV', '15527685070', '2024-07-05 00:00:00', '曹倚铭');
INSERT INTO `camera` VALUES (4, '卧室3', 'G7kL9mN2pZ8rT4uV', '18815541851', '2024-07-05 00:00:00', '韩明昊');
INSERT INTO `camera` VALUES (5, '卧室4', 'X4dR7pL3H9mN2bQ5', NULL, '2024-07-06 15:42:11', '宋剑');
INSERT INTO `camera` VALUES (6, '卧室5', '5JF6Q7GGSE0WDLBP', '15527685070', '2024-07-06 10:48:03', '宋剑');
INSERT INTO `camera` VALUES (7, '卧室6', '5JF6Q7GeSE0WDLBP', '15527685070', '2024-07-06 10:49:03', '宋剑');

SET FOREIGN_KEY_CHECKS = 1;
