#include <bits/stdc++.h>

using namespace std;
#define inf 1000000000
#define unvisited -1
#define visited 1
#define eps 1e-9
#define mp make_pair
#define pb push_back
#define pi acos(-1.0)
#define uint64 unsigned long long
#define FastSlowInput ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
typedef long long ll;
typedef vector<int> vi;
typedef pair<int,int> ii;
typedef pair<ii,int> iii;
typedef vector<ii> vii;

const int maxvertex = 2e6 + 5;

const int numberOfPartition = 3;
const int epsilon = 1;
const double lambda = 1;
int numberOfVertex;
ll partitionSize[maxvertex]; 
vector<set<int> > partitionOfVertices; //vertex is placed in which partitions
ll degree[maxvertex];
int numberOfEdges[numberOfPartition];
int maxLoad = 0;

void setPartitionOfVertices() {
	partitionOfVertices.assign(maxvertex, set<int>());
}

void setup() {
	setPartitionOfVertices();
	set<int>::iterator it;
}

int getIndexOfSmallestPartition(const set<int>& s) {
	const ll MAX_PARTITION_SIZE = 1e18;
	ll minSize = MAX_PARTITION_SIZE;
	int idxMin = -1;
	vi candidateList;
	if(s.empty()) {
		for(int i=0;i<numberOfPartition;i++){
			if((ll)partitionSize[i] < minSize) {
				minSize = partitionSize[i];
				idxMin = i;
				candidateList.clear();
				candidateList.pb(idxMin);
			}else if((ll)partitionSize[i] == minSize) {
				candidateList.pb(i);
			}
		}
	}else{
		set<int>::iterator it;

		for(it=s.begin();it!=s.end();it++){
			int partitionIdx = *it;
			if((ll)partitionSize[partitionIdx] < minSize) {
				minSize = partitionSize[partitionIdx];
				idxMin = partitionIdx;
				candidateList.clear();
				candidateList.pb(idxMin);
			}else if((ll)partitionSize[partitionIdx] == minSize) {
				candidateList.pb(partitionIdx);
			}
		}
	}
	srand(time(NULL));
	int idx = rand()%candidateList.size();
	int partitionId = candidateList[idx];
	return partitionId;
}

set<int> getIntersection(const set<int>& s1, const set<int>& s2) {
	set<int>::iterator it;
	set<int> intersection;
	for(it=s1.begin();it!=s1.end();it++){
		if(s2.find(*it) != s2.end()) {
			intersection.insert(*it);
		}
	}
	return intersection;
}

bool isVertexExistInPartition(int partition, int vertex) {
	return partitionOfVertices[vertex].find(partition) != partitionOfVertices[vertex].end();
}

int getMinSize() {
	int minim = inf;
	for(int i=0;i<numberOfPartition;i++){
		minim = min(minim, numberOfEdges[i]);
	}
	return minim;
}

void incrementMachineLoad(int partitionId) {
	numberOfEdges[partitionId]++;
	maxLoad = max(maxLoad, numberOfEdges[partitionId]);
}

void addPartitionSize(int idxPartition) {
	partitionSize[idxPartition]++;
}

void addToPartition(int vertex, int idxPartition) {
	partitionOfVertices[vertex].insert(idxPartition);
}

void addEdge(int src, int dest, int idxPartition) {
	degree[src]++;
	degree[dest]++;
	incrementMachineLoad(idxPartition);
}

void getTheta(int src, int dest, double &thetaSrc, double &thetaDest) {
	int degreeSrc = degree[src] + 1;
	int degreeDest = degree[dest] + 1;
	int sumDegree = degreeSrc + degreeDest;
	thetaSrc = (double)degreeSrc / (double)sumDegree;
	thetaDest = (double)degreeDest / (double)sumDegree;	
}


int getPartitionNumberOfEdge(int src, int dest) {
	// set<int> s1 = partitionOfVertices[src];
	// set<int> s2 = partitionOfVertices[dest];
	// int idxPartition;
	// if(s1.empty() && s2.empty()) {
	// 	idxPartition = getIndexOfSmallestPartition(s1);
	// }else if(s1.empty() && !s2.empty()) {
	// 	idxPartition = getIndexOfSmallestPartition(s2);
	// }else if(!s1.empty() && s2.empty()) {
	// 	idxPartition = getIndexOfSmallestPartition(s1);
	// }else{
	// 	set<int> intersection = getIntersection(s1, s2);
	// 	if(intersection.empty()){
	// 		if(degree[src] < degree[dest]) {
	// 			idxPartition = getIndexOfSmallestPartition(s1);
	// 		}else{
	// 			idxPartition = getIndexOfSmallestPartition(s2);
	// 		}
	// 	}else{
	// 		idxPartition = getIndexOfSmallestPartition(intersection);
	// 	}
	// }
	// int machineId = idxPartition
	// if(!hasReplicaInPartition(machineId, src)){addToPartition(src, machineId); incrementMachineLoad(machineId);}
	// if(!hasReplicaInPartition(machineId, dest)){addToPartition(dest, machineId); incrementMachineLoad(machineId);}

	// //update edges
	// addEdge(src, dest, machineId);

	// return idxPartition;
	vector<int> candidateList;
	double scoreHDRF = 0;
	int partitionId = -1;

	for(int partition=0;partition < numberOfPartition;partition++) {
		double thetaSrc, thetaDest;
		getTheta(src, dest, thetaSrc, thetaDest);
		double gSrc, gDest;
		if(!isVertexExistInPartition(partition, src)) {
			gSrc = 0;
		}else{
			gSrc = 2.0 - thetaSrc;
		}
		if(!isVertexExistInPartition(partition, dest)) {
			gDest = 0;
		}else{
			gDest = 2.0 - thetaDest;
		}

		double scoreReplicationFactor = gSrc + gDest;
		double scoreBalance = (double)lambda*(double)(maxLoad-numberOfEdges[partition])/(double)(epsilon + maxLoad - getMinSize());
		double totalScore = scoreReplicationFactor + scoreBalance;
		if(totalScore < scoreHDRF) {
			//do nothing
		}else if(totalScore > scoreHDRF) {
			scoreHDRF = totalScore;
			candidateList.clear();
			candidateList.pb(partition);
		}else {
			candidateList.pb(partition);
		}
	}

	//pick one candidate randomly
	srand(time(NULL));
	int idx = rand()%candidateList.size();
	partitionId = candidateList[idx];

	//add vertex to partition
	addToPartition(src, partitionId);
	addToPartition(dest, partitionId);
	addEdge(src, dest, partitionId);

	return partitionId;
}

int stats[numberOfPartition];
vii listEdge;
map<int,int> numDeg;
set<ii> s;
vector<iii> ans;

int main(){
	time_t begin = clock();
	setup();
	// freopen("input.txt", "r", stdin);
	// freopen("output.txt", "w", stdout);
	int src, dest;
	int counter = 0;
	
	while(scanf("%d %d",&src,&dest) != EOF) {
		if(src > dest){swap(src, dest);}
		s.insert(ii(src, dest));
	}
	set<ii>::iterator it;
	for(it=s.begin();it!=s.end();it++){
		listEdge.pb(*it);
	}
	int ukuran = listEdge.size();
	srand(time(NULL));
	random_shuffle(listEdge.begin(), listEdge.end());

	for(int i=0;i<ukuran;i++){
		int src = listEdge[i].first, dest = listEdge[i].second;

		int idxPartition = getPartitionNumberOfEdge(src, dest);
		ans.pb(iii(ii(src,dest), idxPartition));
		if(counter % 100000 == 0){
			cerr<<counter<<endl;
		}
		counter++;
		stats[idxPartition]++;
	}
	time_t end = clock();
	cerr<<"Time taken: "<<(double)(end-begin) / (double)CLOCKS_PER_SEC<<endl;
	sort(ans.begin(), ans.end());
	for(int i=0;i<ukuran;i++){
		iii pas = ans[i];
		printf("%d %d %d\n",pas.first.first, pas.first.second, pas.second);
	}
	for(int i=0;i<numberOfPartition;i++){
		cerr<<i<< " " <<stats[i]<<endl;
	}
	int sum = 0, cnt = 0;
	for(int i=0;i<maxvertex;i++){
		int ukuran = partitionOfVertices[i].size();
		if(ukuran){
			sum += ukuran;
			cnt++;
		}
	}
	cerr<<fixed;
	cerr<<setprecision(4);
	cerr<<"Replication factor: "<<(double)sum/(double)cnt<<endl;
	return 0;
};