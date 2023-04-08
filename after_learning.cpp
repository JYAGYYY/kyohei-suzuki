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

void readweight(string weightfile, vector<vector<vector<double> > > &weight, int &maxlayer, vector<int> &maxnode)
{
    int pos_kan, pos_sep1, pos_sep2;
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

void memoryalloc(vector<vector<double> > &nodeinput, vector<vector<double> > &nodeoutput, int maxlayer, const vector<int> &maxnode){
    int k;
    nodeinput.resize(maxlayer+1);
    for(k=2; k<=maxlayer; k++){
        nodeinput[k].resize(maxnode[k]+1);
    }

    nodeoutput.resize(maxlayer+1);
    for(k=1; k<=maxlayer; k++){
        nodeoutput[k].resize(maxnode[k]+1);
    }
}


int main(int argc, char *argv[]){
    string inputfile_name=argv[1];
    string complete_weight_name=argv[2];
    vector<vector<vector<double>>> weight;
    vector<vector<double> > inputdata;	
    vector<vector<double> > nodeinput;
    vector<vector<double> > nodeoutput;
    vector<double> oneinputdata;
    vector<int> maxnode;
    int maxlayer;

    readweight(complete_weight_name, weight, maxlayer, maxnode);  
    readinout(inputfile_name, inputdata);
    memoryalloc(nodeinput,nodeoutput,maxlayer,maxnode);
 
    for(int k=0; k<inputdata.size(); k++){
        oneinputdata=inputdata[k];
        forward(oneinputdata,weight,maxlayer,maxnode,nodeinput,nodeoutput);
        cout << "****input_data["<<k<<"]_result\nairplane:" << nodeoutput[maxlayer][1]*100 << "%\nship:" << nodeoutput[maxlayer][2] *100 <<"%\n\n";
    }

    return 0;
}

//△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△