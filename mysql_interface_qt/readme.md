## 基于qt封闭的mysql接口
不支持多线程


## 如何使用

```
    MySQLInterface *mysql=new MySQLInterface();

    //1 打开mysql
    //bool isOpen = mysql->openMySql("localhost",3306,"test01","root","A13218343010");
    mysql->openMySql("test01","A13218343010");


    //2 条件查询
           //查询全表 mysql->select("students");
           //条件查询 mysql->select("students","id=4");

    mysql->select("students","id=1");
    //mysql->select("classes");
    //mysql->select("class");

    //3 投影查询,转至外部输出
    QMap<QString,QVector<QString>> mapResult;
    QVector<QString> vecResult;
    mapResult.insert("id",vecResult);
    mapResult.insert("name",vecResult);
    //mysql->select("students",&mapResult);
    for(auto it=mapResult.begin();it!=mapResult.end();++it){
        for(auto itvec=it.value().begin();itvec!=it.value().end();++itvec){
            //cout<<*itvec;
        }
    }

    //插入
    QMap<QString,QString> mapInsert;
    mapInsert["class_id"]="5";
    mapInsert["name"]="张三";
    mapInsert["gender"]="F";
    mapInsert["score"]="66";
    mapInsert["mathscore"]="66";
    //mysql->insert("students",mapInsert);

    //更新
    QMap<QString,QString> mapUpdate;
    mapUpdate["name"]="bbbbbb";
    mapUpdate["score"]="-20";
    //mysql->updata("students",mapUpdate,"id=1");
    //mysql->updata("students2",mapUpdate);   //更新表中所有内容

    //删除
    //mysql->Delete("students2");  //删除所有
    //mysql->Delete("students2","id=1");

    //显示数据库中所有表名
    //mysql->showTable();

    //建表 默认第一个为主键
    QList<QString> colName={"id","name","score"};
    QList<QString> colType={"int(11)","varchar(255)","int(11)"};
    QList<QString> isNull={"not null","not null","null"};
    mysql->creataTabel("students03",colName,colType,isNull);

    //mysql->deleteTable("students03");

    //往表中插一列
    //mysql->addCol("students03","mathscore","int(11)","not null");

    //删除列
    mysql->deteleCol("students03","mathscore");


    //显示
    QTableView *tableView=new QTableView();
    mysql->display(tableView,"students","id>10");

    delete mysql;
```
