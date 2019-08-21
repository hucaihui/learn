#include "mysqlinterface.h"

MySQLInterface::MySQLInterface()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
}

MySQLInterface::~MySQLInterface()
{
    query.exec("EXIT;");
    query.clear();
    db.close();
}

bool MySQLInterface::openMySql(QString strDBname, QString strUserPwd)
{
    return openMySql("",3306,strDBname,"",strUserPwd);  //按默认打开
}

/*************************************************************
 * function:  打开mysql数据库
 * Author  :  Hu CaiHui
 * -----------------------------------------------------------------
 * use     :  openMySql("localhost",3306,"test01","root","123456");
 * Param of Output:
 *                  bool
 * Param of Input:
 *                  strSvrName 主机名或ip地址
 *                  nPort      端口
 *                  strDBname  需要打开的database
 *                  strUserID  用户名
 *                  strUserPwd 用户密码
 **************************************************************/
bool MySQLInterface::openMySql(QString strSvrName,int nPort,QString strDBname,
                             QString strUserID,QString strUserPwd)
{
    if(strSvrName.length() < 1) strSvrName = "localhost";
    if(strUserID.length() < 1) strUserID = "root";
    if(strDBname.length() < 1) return false;

    db.setHostName(strSvrName);
    db.setPort(nPort);
    db.setDatabaseName(strDBname);
    db.setUserName(strUserID);//"root");
    db.setPassword(strUserPwd);
    db.exec("SET NAMES 'utf8'");
    if(false == db.open())
    {
        return false;
    }
    cout<<"open success";
    return true;
}

bool MySQLInterface::tableIsExist(const QString tableName)
{
    if(tableName==m_tableName)   // ==>单例
        return true;
    m_tableName=tableName;

    query.clear();
    query=QSqlQuery(db);
    QString strtmp=QString("SELECT * FROM %1;").arg(tableName);
    //return query.exec(strtmp);
    bool isOK=query.exec(strtmp);
    if(!isOK)
        cout<<"error:this table name is not exist";
    return isOK;

}

int MySQLInterface::getTableRow(const QString tableName){
    tableIsExist(tableName);

    int row=0; //行数
    QString mysqlStrTmp=QString("SELECT count(*) FROM %1;").arg(m_tableName);
    query.exec(mysqlStrTmp);
    while(query.next()){
        row = query.value(0).toInt();
        cout<<"table row number:"<<row;
    }
    return row;
}

int MySQLInterface::getTableCol(const QString tableName){
    tableIsExist(tableName);

    int col=0; //查列数
    QString mysqlStrTmp=QString("SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME='%1';").arg(tableName);
    query.exec(mysqlStrTmp);
    while(query.next()){
        col=query.value(0).toInt();
        cout<<"table column number:"<<col;
    }
    return col;
}

QVector<QString> MySQLInterface::getColumnName(const QString tableName){
    QVector<QString> ret;
    if(!tableIsExist(tableName))
        return ret;
    QString mysqlStrTmp=QString("DESC %1").arg(tableName);
    query.exec(mysqlStrTmp);
    while(query.next()){
        ret.append(query.value(0).toString());
    }
    return ret;
}

QSqlQuery MySQLInterface::select(const QString tableName, const QString whereName)
{
    //查询表是否存在
    if(!tableIsExist(tableName))
        return query;

    QString mysqlStr;   //mysql语句
    if(""==whereName){   //查询全表
        mysqlStr=QString("SELECT * FROM %1;").arg(tableName);
    }
    else{  //按条件查询
        QString strTmp=QString("WHERE %1").arg(whereName);
        mysqlStr=QString("SELECT * FROM %1 %2;").arg(tableName).arg(strTmp);
    }
    query.exec(mysqlStr);
    return query;

    //直接打印输出
//    int selectNum=0;  //查询出来的数据的数据
//    while(query.next()){
//        QString str;
//        for(int i=0;i<col;i++){
//            str += query.value(i).toString();
//            str +=" ";
//        }
//        cout<<str;
//        selectNum++;
//    }
//    cout<<"查询结果总数:"<<selectNum;
}

/*************************************************************
 * function:  mysql投影条件查询
 * Author  :  Hu CaiHui
 * -----------------------------------------------------------------
 * use     :  投影查询      mysql->select("students",&mapResult);
              投影条件查询  mysql->select("students",&mapResult,"id=4");
              内部测试输出  mysql->select("students",nullptr,"id=4");
 * Param of Output:
 *                  bool
 * Param of Input:
 *                  tableName 表名
 *                  whereName 查询条件
 *                  ptr_map   map容器，用作外部输出，存储查询的结果
 **************************************************************/
bool MySQLInterface::select(const QString tableName,
                            QMap<QString,QVector<QString>> *ptr_map,
                            const QString whereName)
{
    if(nullptr==ptr_map){
        select(tableName,whereName);    //ptr_map为空，用作内部测试，直接输出
        cout<<"output in window";
        return true;
    }

    int len=(*ptr_map).size();
    int col=getTableCol(tableName);
    if(len>col){
        cout<<"error:map size to big";
        return false;
    }

    //查询表是否存在
    if(!tableIsExist(tableName))
        return false;

    QString mysqlStr;   //mysql语句
    if(""==whereName){   //查询全表
        mysqlStr=QString("SELECT * FROM %1;").arg(tableName);
    }
    else{  //按条件查询
        QString strTmp=QString("WHERE %1").arg(whereName);
        mysqlStr=QString("SELECT * FROM %1 %2;").arg(tableName).arg(strTmp);
    }
    query.exec(mysqlStr);

    int selectNum=0;  //查询出来的数据的数据
    while(query.next()){
        QString str;
        for(auto it=(*ptr_map).begin();it!=(*ptr_map).end();++it){
            QString str1 = query.value(it.key()).toString();
            str = query.value(it.key()).toString();
            str +=" ";
            (*ptr_map)[it.key()].append(str1);
        }
        cout<<str;
        selectNum++;
    }

    cout<<"查询结果总数:"<<selectNum;

    return true;
}

bool MySQLInterface::insert(const QString tableName,const QMap<QString,QString> mapInsert)
{
    if(!tableIsExist(tableName))
        return false;
    QString strKey,strValue;
    for(auto it=mapInsert.begin();it!=mapInsert.end();++it){
        strKey+=it.key()+",";
        strValue+="'"+it.value()+"'"+",";
    }
    strKey = strKey.left(strKey.length()-1);
    strValue = strValue.left(strValue.length()-1);

    //"INSERT INTO students (class_id, name, gender, score) VALUES (2, 'li', 'M', 80);
    QString mysqlStrTmp=QString("INSERT INTO %1 (%2) VALUES (%3);").arg(tableName).arg(strKey).arg(strValue);
    //cout<<mysqlStrTmp;
    if(query.exec(mysqlStrTmp))
        return true;

    cout<<"insert failure";
    return false;
}

/*************************************************************
 * function:  更新tableName表中的数据
 * Author  :  Hu CaiHui
 * -----------------------------------------------------------------
 * use     :    mysql->updata("students",mapUpdate,"id=1");
                mysql->updata("students2",mapUpdate);   //更新表中所有内容
 * Param of Output:
 *                  bool
 * Param of Input:
 *                  tableName  表名
 *                  mapInsert  map容器，存储需要插入的数据
 *                  whereName  查询条件
 **************************************************************/
bool MySQLInterface::updata(const QString tableName,const QMap<QString,QString> mapUpdata,const QString whereName)
{
    if(!tableIsExist(tableName))
        return false;
    QString strtmp;
    for(auto it=mapUpdata.begin();it!=mapUpdata.end();++it){
        strtmp += it.key() + "='" + it.value() + "',";
    }
    strtmp = strtmp.left(strtmp.length()-1);
    cout<<strtmp;

    QString mysqlStrTmp;
    if(""==whereName){
        QMessageBox::StandardButton rb = QMessageBox::warning(NULL, "warning", "when whereName=\"\" ,will updata all information.\n   Updata all?",
                                                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if(QMessageBox::No==rb){
            return false;
        }
        mysqlStrTmp=QString("UPDATE %1 SET %2;").arg(tableName).arg(strtmp);
    }
    else{
        mysqlStrTmp=QString("UPDATE %1 SET %2 WHERE %3;").arg(tableName).arg(strtmp).arg(whereName);
    }

    //UPDATE students SET name='大牛', score=66 WHERE id=1;
    cout<<mysqlStrTmp;
    if(query.exec(mysqlStrTmp))
        return true;

    cout<<"updata failure";
    return false;
}

/*************************************************************
 * function:  删除tableName表中的数据
 * Author  :  Hu CaiHui
 * -----------------------------------------------------------------
 * use     :    mysql->Delete("students2");  //删除所有
                mysql->Delete("students2","id=1");
 * Param of Output:
 *                  bool
 * Param of Input:
 *                  tableName  表名
 *                  whereName  查询条件
 **************************************************************/
bool MySQLInterface::Delete(const QString tableName,const QString whereName)
{
    if(!tableIsExist(tableName))
        return false;

    QString mysqlStrTmp;
    if(""==whereName){
        QMessageBox::StandardButton rb = QMessageBox::warning(NULL, "warning", "when whereName=\"\" ,will delete all information.\n Delete all?",
                                                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if(QMessageBox::No==rb){
            return false;
        }
        mysqlStrTmp = QString("DELETE FROM %1").arg(tableName);
    }
    else{
        mysqlStrTmp = QString("DELETE FROM %1 WHERE %2").arg(tableName).arg(whereName);
    }

    //DELETE FROM students WHERE id>=5 AND id<=7;
    cout<<mysqlStrTmp;
    if(query.exec(mysqlStrTmp)){

        return true;
    }

    cout<<"delete failure";
    return false;
}

bool MySQLInterface::showTable()
{
    QString mysqlStrTmp="SHOW TABLES;";
    QSqlQuery queryTmp(mysqlStrTmp);
    while (queryTmp.next()) {
        cout<< "table name:" << queryTmp.value(0).toString();
    }
    return true;
}

bool MySQLInterface::creataTabel(const QString tableName,const QList<QString> colName,
                 const QList<QString> colType,const QList<QString> isNull)
{
    if(tableIsExist(tableName)){
        cout<<"the table is exist";
        return false;
    }
    if(colName.size()!=colType.size()||colName.size()!=isNull.size()){
        cout<<"colName.size() must equal to colType.size()";
        return false;
    }
    for(int i=0;i<isNull.size();i++){
        QString s=isNull.at(i).toUpper();
        if(s!="NULL"&&s!="NOT NULL"){
            cout<<"isNull list have some value not be NULL or NOT NULL";
            return false;
        }
    }

    QString strTmp;
    strTmp += "`"+ colName.at(0) + "`" + colType.at(0) +" "+ isNull.at(0) +" "+ "AUTO_INCREMENT"+",";
    for(int i=1;i<colName.size();i++){
        strTmp += "`"+ colName.at(i) + "`" + colType.at(i) +" "+ isNull.at(i) + ",";
    }
    strTmp+= QString("PRIMARY KEY (`%1`)").arg(colName.at(0));

    QString mysqlStrTmp=QString("CREATE TABLE `%1`(%2)ENGINE=InnoDB "
                                "DEFAULT CHARSET=utf8 COLLATE=utf8_bin;").arg(tableName).arg(strTmp);
    //cout<<mysqlStrTmp;
    if(query.exec(mysqlStrTmp))
        return true;
    cout<<"create table failure";
    return false;
}

bool MySQLInterface::deleteTable(const QString tableName){
    if(!tableIsExist(tableName)){
        cout<<"the table is not exist";
        return false;
    }

    QString mysqlStrTmp=QString("DROP TABLE %1;").arg(tableName);
    if(query.exec(mysqlStrTmp))
        return true;
    cout<<"delete success";
    return false;
}

//往表中插入一列
bool MySQLInterface::addCol(const QString tableName,const QString strColName,const QString strColType,const QString strIsNull)
{
    if(!tableIsExist(tableName))
        return false;
    if(strIsNull.toUpper()!="NOT NULL"&&strIsNull.toUpper()!="NULL"){
        cout<<"error:strIsNull must be null or not null";
        return false;
    }
    //ALTER TABLE students ADD COLUMN birth VARCHAR(10) NOT NULL;
    QString mysqlStrTmp=QString("ALTER TABLE %1 ADD COLUMN %2 %3 %4;").arg(tableName).arg(strColName).arg(strColType).arg(strIsNull);
    cout<<mysqlStrTmp;
    if(query.exec(mysqlStrTmp)){
        return true;
    }

    cout<<"add a column failure";
    return false;
}
//删除列
bool MySQLInterface::deteleCol(const QString tableName,const QString strColName)
{
    if(!tableIsExist(tableName))
        return false;
    QVector<QString> vec=getColumnName(tableName);
    for(auto it=vec.begin();it!=vec.end();++it){
        cout<<*it;
    }

    //ALTER TABLE students DROP COLUMN birthday;
    QString mysqlStrTmp=QString("ALTER TABLE %1 DROP COLUMN %2;").arg(tableName).arg(strColName);
    //return query.exec(mysqlStrTmp);
    if(query.exec(mysqlStrTmp)){
        return true;
    }
    cout<<"delete a column failure";
    return false;
}

void MySQLInterface::display(QAbstractItemView *tableView,
                             const QString tableName, const QString whereName)
{
    QSqlQuery queryTmp=select(tableName,whereName);
    QSqlQueryModel *squeryModel=new QSqlQueryModel();
    squeryModel->setQuery(queryTmp);
    tableView->setModel(squeryModel);
    tableView->show();
}
