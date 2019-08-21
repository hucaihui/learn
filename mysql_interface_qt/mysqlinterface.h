#ifndef MYSQLINTERFACE_H
#define MYSQLINTERFACE_H

#include<QDebug>
#include<QString>
#include<QSqlQuery>
#include<QSqlDatabase>
#include<QVector>
#include<QMap>
#include<QMessageBox>
#include<QList>
#include<QTableView>
#include<QSqlQueryModel>
#include<QSqlTableModel>
#include<QAbstractItemView>
#define cout qDebug()<<"["<<__FILE__<<":"<<__LINE__<<"]"

class MySQLInterface
{
public:
    MySQLInterface();
    ~MySQLInterface();
    bool openMySql(QString strDBname, QString strUserPwd);
    bool openMySql(QString strSvrName,int nPort,QString strDBname,
                    QString strUserID,QString strUserPwd);

    /**********************************对表中内容操作********************************************/
    //获取表行数
    int getTableRow(const QString tableName);
    //获取表列数
    int getTableCol(const QString tableName);
    //获取表所有的列名
    QVector<QString> getColumnName(const QString tableName);
    //查询
    QSqlQuery select(const QString tableName, const QString whereName="");
    bool select(const QString tableName,QMap<QString,QVector<QString>> *ptr_map,const QString whereName="");
    //插入
    bool insert(const QString tableName,const QMap<QString,QString> mapInsert);
    //更新
    bool updata(const QString tableName,const QMap<QString,QString> mapUpdata,const QString whereName="");
    //删除
    bool Delete(const QString tableName,const QString whereName="");
    //往表中插入一列
    bool addCol(const QString tableName,const QString strColName,const QString strColType,const QString strIsNull);
    //删除列
    bool deteleCol(const QString tableName,const QString strColName);
    /*************************************************************************************/
    //显示所有数据库中所有表名
    bool showTable();
    //建表
    bool creataTabel(const QString tableName,const QList<QString> colName,
                     const QList<QString> colType,const QList<QString> isNull);
    //删表
    bool deleteTable(const QString tableName);

    //显示至tableview中
    void display(QAbstractItemView *tableView,const QString tableName, const QString whereName="")
    {
        QSqlQuery queryTmp=select(tableName,whereName);
        QSqlQueryModel *squeryModel=new QSqlQueryModel();
        squeryModel->setQuery(queryTmp);
        tableView->setModel(squeryModel);
        tableView->show();
    }
protected:

private:
    bool tableIsExist(const QString tableName);

private:
    QSqlDatabase db;
    QSqlQuery query;
    QString m_tableName;
};

#endif // MYSQLINTERFACE_H
