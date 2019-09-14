#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

vector<vector<int>> bag(const int W,vector<int> weigths, vector<int> values) {
	//dp数组初始化 size=num*(W+1)
	int num = weigths.size();
	vector<vector<int>> dp;
	vector<int> tmp(W+1, 0);
	for (int i = 0; i < num; ++i) {
		dp.push_back(tmp);
	}
	for (int i = 1; i < num; ++i) {
		for (int j = 1; j <= W; ++j) {
			if (j < weigths[i]) {
				dp[i][j] = dp[i - 1][j];
			} else {
				dp[i][j] = max(dp[i - 1][j], dp[i - 1][j - weigths[i]] + values[i]);
			}
		}
	}
	return dp;
};

void findWhat(vector<vector<int>> dp,vector<int> w,vector<int> v,int i, int j) {
	if (i > 0) {
		if (dp[i][j] == dp[i - 1][j]) {
			findWhat(dp,w,v,i - 1, j);
		} else if(j-w[i]>=0&&dp[i][j]==dp[i-1][j-w[i]]+v[i]){
			cout << i << " ";    //回溯值
			findWhat(dp,w,v,i - 1,j - w[i]);
		}
	}
}
int main() {
	const int W = 8;
	vector<int> weigths = {0, 2,3,4,5};
	vector<int> values = {0,3,5,4,6};

	vector<vector<int>> dp = bag(W, weigths, values);
	for (int i = 0; i < dp.size(); ++i) {
		for (int j = 0; j < dp[0].size(); ++j) {
			cout << dp[i][j] << " ";
		}
		cout << endl;
	}
	cout << dp[4][8] << endl;
	findWhat(dp, weigths, values, 4, 8);

	system("pause");
	return 0;
}
