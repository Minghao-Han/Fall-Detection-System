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

 Date: 12/07/2024 17:02:12
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for admin
-- ----------------------------
DROP TABLE IF EXISTS `admin`;
CREATE TABLE `admin`  (
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '姓名',
  `password` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '密码',
  `phone` varchar(11) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '电话',
  PRIMARY KEY (`phone`) USING BTREE,
  UNIQUE INDEX `phone`(`phone` ASC) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci COMMENT = '用户信息表' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of admin
-- ----------------------------
INSERT INTO `admin` VALUES ('momo', '123456', '11111111111');
INSERT INTO `admin` VALUES ('sj', '123456', '12345678900');
INSERT INTO `admin` VALUES ('宋剑', '123456', '13675542871');
INSERT INTO `admin` VALUES ('张三', '123456', '15527685070');
INSERT INTO `admin` VALUES ('李四', '123456', '18815541851');

-- ----------------------------
-- Table structure for camera
-- ----------------------------
DROP TABLE IF EXISTS `camera`;
CREATE TABLE `camera`  (
  `cameraName` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '摄像机的名字',
  `serialNumber` varchar(16) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '摄像机的序列号',
  `phone` varchar(11) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '绑定用户电话',
  `dateTime` datetime NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '绑定的日期',
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '老人的名字',
  PRIMARY KEY (`serialNumber`) USING BTREE,
  UNIQUE INDEX `serialnumber`(`serialNumber` ASC) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of camera
-- ----------------------------
INSERT INTO `camera` VALUES ('卧室11', '5JF6Q7GeSE0WDLBP', '13675542871', '2024-07-11 00:00:00', '宋剑');
INSERT INTO `camera` VALUES ('卧室11', '5JF6Q7GGSE0WDLBK', '15527685070', '2024-07-05 00:00:00', '宋剑');
INSERT INTO `camera` VALUES ('卧室5', '5JF6Q7GGSE0WDLBP', '15527685070', '2024-07-06 10:48:03', '宋剑');
INSERT INTO `camera` VALUES ('寝室', 'A7kL9mN2pQ8rT4uV', '15527685070', '2024-07-06 00:00:00', '张三');
INSERT INTO `camera` VALUES ('卧室2', 'G7kL9mN2pQ8rT4uV', '15527685070', '2024-07-05 00:00:00', '曹倚铭');
INSERT INTO `camera` VALUES ('卧室3', 'G7kL9mN2pZ8rT4uV', '18815541851', '2024-07-05 00:00:00', '韩明昊');
INSERT INTO `camera` VALUES ('客厅2', 'X4dR7pL3H9mN2bQ1', '15527685070', '2024-07-11 17:04:24', '王立');
INSERT INTO `camera` VALUES ('卧室4', 'X4dR7pL3H9mN2bQ5', '15527685070', '2024-07-06 15:52:09', '宋剑');

SET FOREIGN_KEY_CHECKS = 1;
