// ----------------------------------------------------------------- //
//                         Back Propagation                          //
// ----------------------------------------------------------------- //

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


using namespace std;

const double coef=0.7; //学習係数



// 第1引数は、入力層もしくは出力層の学習データが含まれるファイルを指定する
// ファイルは、行ごとにカンマ区切りで対応する入力パターン・出力パターンが記載されている
// 第2引数は、第1引数で指定されたファイルのデータが入る2次元のvector形式の変数を指定する
// 2次元のデータは、最初のインデックスが行数を表し、2番目のインデックスが各ノードの値を示す
// 例えば、inoutdata[i][j]は、inoutfileのi+1行目、j番目のノードのデータを格納する
// このため、iは0から、jは1からとなる
void readinout(string inoutfile, vector<vector<double> > &inoutdata){
#ifdef PUSHBACK

    string line, tmp;
    long unsigned int start, end;
    vector<double> tmpgyou;

    ifstream fin(inoutfile.c_str());
    if(!fin){
        cerr << "ERROR:cannot open " << inoutfile << endl;
        exit(EXIT_FAILURE);
    }
    while(getline(fin,line)){
        line+=",";											//文末にカンマを追加する
        vector<double> tmpgyou;
        tmpgyou.push_back(1.0);								//ダミーで0番目を空ける(実際には使わない)
        start=0;											
        end=line.find_first_of(",",start);					//カンマを探す。見つかればendにカンマの入っているインデックスが入る。
        while(end !=string::npos){							//カンマがなくなったら終わり
            tmp.assign(line, start, end-start);				//lineのstartからend-1(=カンマの1つ手前)までをtmpにコピー
            tmpgyou.push_back( atof(tmp.c_str()) );			//tmpをdoubleに変換した値を、tmpgyouに追加
            start=end+1;									//startをカンマの次のインデックスに更新
            end=line.find_first_of(",",start);				//次のカンマを探す
        }
        inoutdata.push_back(tmpgyou);						//現時点でのinoutdataの最後の行にtmpgyouを追加
    }
    fin.close();

#else

    string line, tmp;
    int count, p, i;
    long unsigned int start, end;

    ifstream fin(inoutfile.c_str());
    if(!fin){
        cerr << "ERROR:cannot open " << inoutfile << endl;
        exit(EXIT_FAILURE);
    }
    count=0;
    while(getline(fin,line)){								//ファイルの中身を1行ずつlineに読み込む
        count++;
    }
    inoutdata.resize(count);								//行数(=学習データの数)分、メモリを確保

    fin.clear();											//EOFまでいっているのでクリアが必要
    fin.seekg(0,ios_base::beg);								//読み込みを行うため、ファイルポインタをファイル冒頭に戻す
    p=0;
    while(getline(fin,line)){								//ファイルの中身を1行ずつlineに読み込む
        line+=",";											//文末にカンマを追加する
        count=0;
        start=0;											
        end=line.find_first_of(",",start);					//カンマを探す。見つかればendにカンマの入っているインデックスが入る。
        while(end !=string::npos){							//カンマがなくなったら終わり
            start=end+1;									//startをカンマの次のインデックスに更新
            end=line.find_first_of(",",start);				//次のカンマを探す
            count++;										//列の数を増やす
        }
        inoutdata[p].resize(count+1);						//列の数+1のメモリを確保。i番目のノードへの入力がinoutdata[p][i]に入る

        i=1;												//メモリを確保したので実際に値を代入する。1番目から詰めたいのでi=1に設定。
        start=0;											
        end=line.find_first_of(",",start);					//カンマを探す。見つかればendにカンマの入っているインデックスが入る。
        while(end !=string::npos){							//カンマがなくなったら終わり
            tmp.assign(line, start, end-start);				//lineのstartからend-1(=カンマの1つ手前)までをtmpにコピー
            inoutdata[p][i]=atof(tmp.c_str());				//tmpをdoubleに変換した値を、inoutdata[p][i]に代入
            start=end+1;									//startをカンマの次のインデックスに更新
            end=line.find_first_of(",",start);				//次のカンマを探す
            i++;
        }
        p++;												//行内のデータ読み込み終わりなので、行数をインクリメント
    }
    fin.close();

#endif

    return;
}

//▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽

// 第1引数は、各層の重みが含まれるファイルを指定する
// ファイルの形式は、別途サンプルを参照のこと
// 第2引数は、第1引数で指定されたファイルのデータが入る3次元のvector形式の変数を指定する
// 3次元のデータは、最初のインデックスが層を表し、2番目のインデックスがその層の何番目のノードかを表し、3番目のインデックスが前の層の何番目ノードかを表す
// 例えば、weight[k][i][j]は、k-1層目のj番目のノードから、k層目のi番目のノードへの重みを表す。
// kは2から、iは1から、jは0から、となる(j=0は1を出し続ける0番目のノードを表す)
// 第3引数は、層の最大値で、ファイルから読んだ値を関数内部で代入して設定する
// 第4引数は、各層におけるノードのインデックスの最大値で1次元のvector形式の変数を指定する。インデックスは何番目の層かを表す
// 例えば、maxnode[k]は、k層目のノードのインデックスの最大値を表す。kは1からとなる。
void readweight(string weightfile, vector<vector<vector<double> > > &weight, int &maxlayer, vector<int> &maxnode)
{
    int pos_kan;
    int pos_sep1, pos_sep2;
    int k{1};
    int l;
    bool flag=false;
    string s;
    ifstream input_file(weightfile);
    
    while(getline(input_file, s))
    {
        pos_sep1 = s.find_first_of('<');
        pos_sep2 = s.find_first_of('>');
        pos_kan = s.find_first_of(',');
        string type, num;

        if(pos_sep1 != string::npos)
        {
            type = s.substr(pos_sep1+1, pos_sep2-pos_sep1-1);
            
            if(type != "numlayer")
            {
                k++;
                maxnode.resize(k);
                string dou = s.substr(pos_kan+1);
                maxnode.push_back(stoi(dou));
                l = 1;
            }
            else
            {
                string dou = s.substr(pos_kan+1);
                maxlayer = stoi(dou);
            }
        }
        
        else
        {
            int f_can;
            string str=s;
            int count=0;
            
            if(!flag)
            {
                while(f_can != string::npos)
                {
                    f_can=str.find(',');
                    str=str.substr(f_can+1);
                    count++;
                }
                maxnode[1]=count-1;   
                flag=true;
            }

            weight.resize(k+1);
            weight[k].resize(l+1);
            for(int j=0; j<maxnode[k-1]+1; j++)
            {
                if(j == 0)
                {
                    num = s.substr(0, pos_kan);
                    weight[k][l].push_back(stod(num));
    
                }
                else if(j == maxnode[k-1])
                {
                    num = s.substr(pos_kan+1);
                    weight[k][l].push_back(stod(num));
                    l++;
                }
                else
                {
                    int pos_next_kan = s.find(',', pos_kan+1);
                    num = s.substr(pos_kan+1, pos_next_kan-pos_kan-1);
                    weight[k][l].push_back(stod(num));
                    pos_kan = pos_next_kan;
                }
            } 
        }
    }
}


// readweightで読み込んだ形式と、全く同じ形式で出力を行う
// したがって、重みが変化しなければ、入力ファイルと出力ファイルは完全に一致する(diffの出力が何もでない)
// 第1引数は、出力ファイル名
// 第2引数は、3次元のvector形式の重み
// 第3引数は、層の最大値
// 第4引数は、各層におけるノードのインデックスの最大値(1次元のvector形式の変数)
void writeweight(string outfilename, const vector<vector<vector<double> > > &weight, int maxlayer, const vector<int> &maxnode){ 
    ofstream output_file;
    output_file.open(outfilename);
    output_file << "<numlayer>," << maxlayer <<"\n";
    
    for(int k=2; k<=maxlayer; k++){
        output_file << "<layer" << k << ">," << maxnode[k] <<"\n";
        for(int i=1; i<=maxnode[k]; i++){
            for(int j=0; j<=maxnode[k-1]; j++){
                output_file << weight[k][i][j];
                if(j != maxnode[k-1]) output_file << ",";
                else if(i != maxnode[k]) output_file << "\n";
            }
        }
        output_file << "\n";
    }

    output_file.close();
}


// 下記のエラーをチェックする
// p番目の学習データinputdata[p]の大きさ-1が、maxnode[1]に一致していない(0番目のノードの分をひく)
// p番目の学習データoutputdata[p]の大きさ-1が、maxnode[maxlayer]に一致していない(0番目のノードの分をひく)
// weightの大きさが、maxlayer+1に一致していない
// weight[k]の大きさが、maxnode[k]+1に一致していない
// weight[k][i]の大きさが、maxnode[k-1]+1に一致していない
void isconsist(int maxlayer, const vector<int> &maxnode, const vector<vector<double> > &inputdata, const vector<vector<double> > &outputdata, const vector<vector<vector<double> > > &weight){

    for(int p=0;p<int(inputdata.size());p++){											// 学習データごとに調べる
        if((int(inputdata[p].size())-1) !=maxnode[1]){									// inputdata[p]-1の大きさがmaxnode[1]に一致していない場合エラー
            cerr << "ERROR:inputdata size is different:" << p << endl;
            exit(EXIT_FAILURE);
        }
    }

    for(int p=0;p<int(outputdata.size());p++){											// 学習データごとに調べる
        if((int(outputdata[p].size())-1) !=maxnode[maxlayer]){							// outputdata[p]-1の大きさがmaxnode[maxlayer]に一致していない場合エラー
            cerr << "ERROR:outputdata size is different:" << p << endl;
            exit(EXIT_FAILURE);
        }
    }

    if(int(weight.size()) !=(maxlayer+1)){												// weightの大きさがmaxlayer+1に一致していない場合エラー
        cerr << "ERROR:weight size is different:" << endl;
        exit(EXIT_FAILURE);
    }
    for(int k=2;k<=maxlayer;k++){														// 層ごとに調べる
        if(int(weight[k].size()) !=(maxnode[k]+1)){								// weight[k]の大きさがmaxnode[k]+1に一致していない場合エラー
            cerr << "ERROR:weight size is different from maxnode:" << k << endl;
            exit(EXIT_FAILURE);
        }
        for(int i=1;i<=maxnode[k];i++){													// k層のノードごとに調べる
            if(int(weight[k][i].size()) !=(maxnode[k-1]+1)){							// weight[k][i]の大きさがmaxnode[k-1]+1に一致していない場合エラー
                cerr << "ERROR:node size is different:" << k << " "  << i << endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    return;

}

// 各層のノードの入力値nodeinput(=講義資料u)、出力値nodeoutput(=講義資料x)、誤差関数err(=講義資料ε)のメモリをとる
// 重みの増減量deltaweight(=講義資料Δw)のメモリをとる
void memoryalloc(vector<vector<double> > &nodeinput, vector<vector<double> > &nodeoutput, vector<vector<double> > &err, vector<vector<vector<double> > > &deltaweight, int maxlayer, const vector<int> &maxnode){
    int k;
    nodeinput.resize(maxlayer+1);
    for(k=2; k<=maxlayer; k++){
        nodeinput[k].resize(maxnode[k]+1);
    }

    nodeoutput.resize(maxlayer+1);
    for(k=1; k<=maxlayer; k++){
        nodeoutput[k].resize(maxnode[k]+1);
    }

    err.resize(maxlayer+1);
    for(k=2; k<=maxlayer; k++){
        err[k].resize(maxnode[k]+1);
    }

    deltaweight.resize(maxlayer+1);
    for(k=2; k<=maxlayer; k++){
        deltaweight[k].resize(maxnode[k]+1);
        for(int i=1; i<=maxnode[k]; i++){
            deltaweight[k][i].resize(maxnode[k-1]+1);
        }
    }

}

//出力層の値を算出する
void forward(const vector<double> &oneinputdata, const vector<vector<vector<double> > > 
&weight, int maxlayer, const vector<int> &maxnode, vector<vector<double> > &nodeinput, vector<vector<double> > &nodeoutput){
    for(int i=1; i<=maxnode[1]; i++){
        nodeoutput[1][i] = oneinputdata[i];
    }
    for(int k=2; k<=maxlayer; k++){
        for(int i=1; i<=maxnode[k]; i++){
            nodeinput[k][i]=0;
            for(int j=0; j<=maxnode[k-1]; j++){
                nodeinput[k][i]+=nodeoutput[k-1][j]*weight[k][i][j];

            }
            nodeoutput[k][i]=1/(1+exp(-nodeinput[k][i]));
        }
    }
} 

//出力層のデータと教師データの誤差をもとに重みを更新する
void backward(const vector<double> &oneoutputdata, const vector<vector<vector<double> > > &weight, int maxlayer, const vector<int> &maxnode, const vector<vector<double> > &nodeinput, const vector<vector<double> > &nodeoutput, vector<vector<double> > &err){
    double sum=0;
    int k=maxlayer;
    for(int i=1; i<=maxnode[k]; i++){
        if(k==maxlayer){
            err[k][i]=1/(1+exp(-nodeinput[k][i]))*(1-(1/(1+exp(-nodeinput[k][i]))))*(oneoutputdata[i]-nodeoutput[k][i]);
        }
    }
    k--;
    for(; 2<=k; k--){
        for(int j=1; j<=maxnode[k]; j++){
            for(int i=1; i<=maxnode[k+1]; i++){
                sum += weight[k+1][i][j]*err[k+1][i];
            }
            err[k][j]=1/(1+exp(-nodeinput[k][j]))*(1-(1/(1+exp(-nodeinput[k][j]))))*sum;
            sum=0;
        }
    }
} 

// 使用予定のすべてのdeltaweightを0.0に設定
void initdeltaweight(int maxlayer, const vector<int> &maxnode, vector<vector<vector<double> > > &deltaweight){
    for(int k=2; k<=maxlayer; k++){
        for(int i=1; i<=maxnode[k]; i++){
            for(int j=0; j<=maxnode[k-1]; j++){
                deltaweight[k][i][j]=0.0;
            }
        }
    }

}

// 学習データ1つに対するdeltaweightの増減分を計算し、deltaweightに足しこむ
// その際、この段階では学習係数の掛け算は行わない
void accumulator(int maxlayer, const vector<int> &maxnode, const vector<vector<double> > &nodeoutput, const vector<vector<double> > &err, vector<vector<vector<double> > > &deltaweight){
     for(int k=2; k<=maxlayer; k++){
        for(int i=1; i<=maxnode[k]; i++){
            for(int j=0; j<=maxnode[k-1]; j++){
                deltaweight[k][i][j]+=nodeoutput[k-1][j]*err[k][i];
            }
        }
    }
}

// 重みをdeltaweightだけ増減させる
// 学習係数の掛け算はここで行う
void updateweight(int maxlayer, const vector<int> &maxnode, vector<vector<vector<double> > > &weight, const vector<vector<vector<double> > > &deltaweight){
    for(int k=2; k<=maxlayer; k++){
        for(int i=1; i<=maxnode[k]; i++){
            for(int j=0; j<=maxnode[k-1]; j++){
                weight[k][i][j]+=coef*deltaweight[k][i][j];
            }
        }
    }

}

//△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△

int main(int argc, char *argv[])
{

    int numepoch=10;									//学習の繰り返し回数(デフォルト10。オプションEで指定して変更可能)
    int flag=1;											//テストデータモードのときは重みを更新しないことを表すフラグ(オプションTで出力ファイルが指定されることによりテストデータモードになる)
    string inputfilename="";							//入力層の学習データが格納されたファイル名(デフォルト空ファイル。オプションIで変更可能)
    string weightfilename="";							//重みが格納されたファイル名(デフォルト空ファイル。オプションWで変更可能)
    string outputfilename="";							//出力層の学習データが格納されたファイル名(デフォルト空ファイル。オプションOで変更可能)
    string weightoutfilename="";						//学習後の重みを出力するファイル名(デフォルト空ファイルだが最後に指定)
    string nodeoutputfilename="";						//出力層のノードの出力値を出力するファイル名(デフォルト空ファイル。オプションTで変更可能。この指定によりテストデータモードになり重みの値は更新しない)
    int maxlayer;										//入力層を1層目としたときの最終層(=出力層)が何層目かを表す
    vector<int> maxnode;								//各層において、最後のノードが1番目のノードから数えて何番目かを表す
    vector<vector<double> > inputdata;					//inputdata[p][i]で、p+1番目の学習データの入力層i番目のノードの出力値を表す
    vector<vector<vector<double> > > weight;			//重み。weight[k][i][j]で、k層目のi番目における、k-1層目のj番目のノードの重みを表す
    vector<vector<vector<double> > > deltaweight;		//重みの変化量。deltaweight[k][i][j]で、k層目のi番目における、k-1層目のj番目のノードの重みの変化量を表す
    vector<vector<double> > outputdata;					//outputdata[p][i]で、p+1番目の学習データの最終層i番目のノードの出力値の正解を表す
    vector<vector<double> > nodeinput;					//nodeinput[k][i]で、p番目の学習データを入力した時の、k層i番目のノードへの入力値を表す
    vector<vector<double> > nodeoutput;					//nodeoutput[k][i]で、p番目の学習データを入力した時の、k層i番目のノードの出力値を表す
    vector<vector<double> > err;						//err[k][i]で、p番目の学習データを入力した時の、k層i番目のノードのイプシロン値を表す
    double totalerror;									//最終層全ノードにおける全学習データの自乗誤差の和E
    double tmp;

    //「./backpropagation -E 50000 -I inputfile.txt -W weight_init.txt -O outputfile.txt weight.txt」で実行可能なように、argc=10の場合のみ動作し、
    //それ以外の時はエラーを出力するようにする。
    if((argc !=10)&&(argc !=11)){
        cerr << "ERROR" << endl;
        cerr << "./backpropagation -E 50000 -I inputfile.txt -W weight_init.txt -O outputfile.txt weight.txt OR" << endl;
        cerr << "./backpropagation -E 1 -I testinputfile.txt -W weight.txt -O testoutputfile.txt -T nodeoutput.txt" << endl;
        exit(EXIT_FAILURE);
    }else{
        for(int i=1;i<argc;i++){
            if(argv[i][0]=='-'){
                switch(argv[i][1]){
                    case 'E':
                        i++;
                        numepoch=atoi(argv[i]);
                        break;
                    case 'I':
                        i++;
                        inputfilename=argv[i];
                        break;
                    case 'O':
                        i++;
                        outputfilename=argv[i];
                        break;
                    case 'W':
                        i++;
                        weightfilename=argv[i];
                        break;
                    case 'T':
                        i++;
                        nodeoutputfilename=argv[i];
                        cerr << "WARNING:-Wオプションで指定された重みファイルの評価を行います。このため重みは更新されず出力もされません。" << endl;
                        flag=0;
                        cerr << "WARNING:numepochは強制的に1になります" << endl;
                        numepoch=1;
                        break;
                    default:
                        cerr << "このオプションはありません" << endl;
                        cerr << argv[i] << endl;
                        exit(EXIT_FAILURE);
                }
            }else if(i==(argc-1)){
                weightoutfilename=argv[i];
            }else{
                cerr << "argv[i][0]はマイナスでないといけません" << endl;
                cerr << argv[i] << endl;
                cerr << "i=" << i << ":argc=" << argc << endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    // shokika
    if(flag==0){
        ofstream fout(nodeoutputfilename.c_str());
        if(!fout){
            cerr << "エラー：ファイルを開けません" << nodeoutputfilename << endl;
            exit(EXIT_FAILURE);
        }
        fout.close();
    }

    //▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽

    // read weight
    cout << "*****weight\n";
    readweight(weightfilename, weight, maxlayer, maxnode);

    // read input
    cout << "*****inputdata\n";
    readinout(inputfilename,inputdata);
    for(long unsigned int p=0;p<inputdata.size();p++){
        cout << inputdata[p][1];
        for(long unsigned int i=2;i<inputdata[p].size();i++){
            cout << "," << inputdata[p][i];
        }
        cout << "\n";
    }

    // read output
    cout << "*****outputdata\n";
    readinout(outputfilename,outputdata);
    for(long unsigned int p=0;p<outputdata.size();p++){
        cout << outputdata[p][1];
        for(long unsigned int i=2;i<outputdata[p].size();i++){
            cout << "," << outputdata[p][i];
        }
        cout << "\n";
    }

    isconsist(maxlayer,maxnode,inputdata,outputdata,weight);

    // memory allocation for calculation
    memoryalloc(nodeinput,nodeoutput,err,deltaweight,maxlayer,maxnode);
 
    // initialize for unit 0 (thresholds)
    for(int i=1;i<=maxlayer;i++){
        nodeoutput[i][0]=1.0;
    }

    vector<double> oneinputdata;
    vector<double> oneoutputdata;
    double E=0.0;
    double e=0.0;
    int nump;

    for(nump=0; nump<numepoch; nump++){
        for(int k=0; k<inputdata.size(); k++){
            oneinputdata=inputdata[k];
            oneoutputdata=outputdata[k];
            forward(oneinputdata,weight,maxlayer,maxnode,nodeinput,nodeoutput);
            backward(oneoutputdata,weight,maxlayer,maxnode,nodeinput,nodeoutput,err);
            initdeltaweight(maxlayer, maxnode, deltaweight);
            accumulator(maxlayer, maxnode, nodeoutput, err, deltaweight);
            updateweight(maxlayer, maxnode, weight, deltaweight);

            for(int i=1; i<=maxnode[maxlayer]; i++){
                e+=(oneoutputdata[i]-nodeoutput[maxlayer][i])*(oneoutputdata[i]-nodeoutput[maxlayer][i]);
            }
            E+=e;
        }
        E=E*0.5/inputdata.size()/maxnode[maxlayer];
        if(E<=0.01){
            break;
        }
        e=0.0;
        E=0.0;
    }
    
    if(flag !=0){
        writeweight(weightoutfilename, weight, maxlayer, maxnode);
    }

    cout << "*****learn_finish\n";
    cout << "numepoch:"<<nump<<"\n";

    for(int k=0; k<inputdata.size(); k++){
        oneinputdata=inputdata[k];
        forward(oneinputdata,weight,maxlayer,maxnode,nodeinput,nodeoutput);
        cout << "data:" << k+1 << "\n";
        for(int i=1;i<=maxnode[maxlayer];i++){
            cout << maxlayer << " " << i << " " << nodeoutput[maxlayer][i] << "\n";
        }
    }

    //△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△
   
    return 0;
}
