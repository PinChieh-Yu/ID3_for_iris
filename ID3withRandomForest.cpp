#include<iostream>
#include<ctime>
#include<cmath>
#include<fstream>
#include<algorithm>
#include<vector>
using namespace std;
class IrisData{
	public:
		double Attribute[4]; //0:SepalLength 1:SepalWidth 2:PetalLength 3:PetalWidth
		int IrisClass; //0:"Iris-setosa" 1:"Iris-versicolor" 2:"Iris-virginica"
};
class Node{
	public:
		int Result;
		int Attribute;
		double Threshold;
		Node* Left;
		Node* Right; //right:>= left:<
};
void BuildDecisionTree(Node* &node, vector<IrisData> set);
double GetEntropy(vector<IrisData> set);
double GetInformationGain(vector<IrisData> set, double entropy, int attribute, double &threshold);
void GetPerformance(vector<IrisData> set, Node** tree, double* ansset);
int gattri;
bool comp(const IrisData &a, const IrisData &b){
	return a.Attribute[gattri] < b.Attribute[gattri];
}
int main(int argc, char** argv){
	srand(time(NULL));
	fstream fin;
	fin.open(argv[1], ios::in);
	vector<IrisData> TotalDataSet, TrainingDataSet, TestingDataSet;
	Node* Root[5];
	double AnswerSet[7];
	for(int i = 0; i < 150; i++){
		IrisData data;
		string irisclass;
		char s;
		fin >> data.Attribute[0] >> s >> data.Attribute[1] >> s >> data.Attribute[2] >> s >> data.Attribute[3] >> s >> irisclass;
		if(irisclass == "Iris-setosa") data.IrisClass = 0;
		else if(irisclass == "Iris-versicolor") data.IrisClass = 1;
		else data.IrisClass = 2;
		TotalDataSet.push_back(data);
	}
	random_shuffle(TotalDataSet.begin(), TotalDataSet.end());
	for(int i = 0; i < 150; i++){
		if(i < 30) TestingDataSet.push_back(TotalDataSet[i]);
		else TrainingDataSet.push_back(TotalDataSet[i]);
	}
	for(int i = 0; i < 5; i++){
		random_shuffle(TrainingDataSet.begin(), TrainingDataSet.end());
		BuildDecisionTree(Root[i], TrainingDataSet);
	}
	GetPerformance(TestingDataSet, Root, AnswerSet);
	cout << AnswerSet[0] << endl;
	for(int i = 0; i < 3; i++) cout << AnswerSet[i*2+1] << " " << AnswerSet[i*2+2] << endl;
}
void BuildDecisionTree(Node* &node, vector<IrisData> set){
	node = new Node();
	node->Result = set[0].IrisClass;
	for(int i = 0; i < set.size(); i++){
		if(set[i].IrisClass != node->Result){
			node->Result = -1;
			break;
		}
	}
	if(node->Result != -1) return;
	double entropy = GetEntropy(set);
	double infogain;
	double threshold = 0.0;
	double maxinfogain = -1;
	for(int i = 0; i < 4; i++){
		infogain = GetInformationGain(set, entropy, i, threshold);
		if(infogain > maxinfogain){
			maxinfogain = infogain;
			node->Attribute = i;
			node->Threshold = threshold;
		}
	}
	vector<IrisData> left, right;
	for(int i = 0; i < set.size(); i++){
		if(set[i].Attribute[node->Attribute] >= node->Threshold) right.push_back(set[i]);
		else left.push_back(set[i]);
	}
	BuildDecisionTree(node->Left, left);
	BuildDecisionTree(node->Right, right);
	return;
}
double GetEntropy(vector<IrisData> set){
	double entropy = 0;
	double classappearance[3] = {0.0};
	for(int i = 0; i < set.size(); i++) classappearance[set[i].IrisClass]++;
	for(int i = 0; i < 3; i++){
		classappearance[i]/=set.size();
		if(classappearance[i]) entropy -= classappearance[i]*log2(classappearance[i]);
	}
	return entropy;
}
double GetInformationGain(vector<IrisData> set, double entropy, int attribute, double &threshold){
	gattri = attribute;
	sort(set.begin(), set.end(), comp);
	double infogain, maxinfogain = -1;
	vector<IrisData> ssubset;
	for(int i = 0; i < set.size()-1; i++){
		ssubset.push_back(set[i]);
		if(set[i].IrisClass != set[i+1].IrisClass && set[i].Attribute[attribute] != set[i+1].Attribute[attribute]){
			vector<IrisData> bsubset;
			for(int j = i+1; j < set.size(); j++) bsubset.push_back(set[j]);
			infogain = entropy-((ssubset.size()*GetEntropy(ssubset)+bsubset.size()*GetEntropy(bsubset))/set.size());
			if(infogain > maxinfogain){
				maxinfogain = infogain;
				threshold = (set[i].Attribute[attribute]+set[i+1].Attribute[attribute])/2;
			}
		}
	}
	return maxinfogain;
}
void GetPerformance(vector<IrisData> set, Node** tree, double* ansset){
	ansset[0] = 0;
	for(int i = 0; i < 3; i++){
		double prediction[3] = {0}; //0:TP 1:FP 2:FN
		for(int j = 0; j < set.size(); j++){
			int voting[3] = {0}, votenumber = -1, votingresult = 0;
			for(int k = 0; k < 5; k++){
				Node* node = tree[k];
				while(node->Result == -1){
					if(set[j].Attribute[node->Attribute] >= node->Threshold) node = node->Right;
					else node = node->Left;
				}
				voting[node->Result]++;
			}
			for(int k = 0; k < 3; k++){
				if(voting[k] > votenumber){
					votenumber = voting[k];
					votingresult = k;
				}
			}
			if(set[j].IrisClass == i && votingresult == i) prediction[0]++;
			else if(set[j].IrisClass != i && votingresult == i) prediction[1]++;
			else if(set[j].IrisClass == i && votingresult != i) prediction[2]++;
		}
		ansset[i*2+1] = prediction[0]/(prediction[0]+prediction[1]);
		ansset[i*2+2] = prediction[0]/(prediction[0]+prediction[2]);
		ansset[0] += prediction[0];
	}
	ansset[0] /= set.size();
}
