#include "./DetecResult.h"
bool BoxCompare(
	const DetectBox& a,
	const DetectBox& b) {
	return a.score > b.score;
}