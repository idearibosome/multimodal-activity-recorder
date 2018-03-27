#ifndef IRQMSQLITEHELPER_H
#define IRQMSQLITEHELPER_H

#include <QObject>
#include <QVariant>

struct sqlite3;
struct sqlite3_stmt;

class IRQMSQLiteHelper : public QObject
{
    Q_OBJECT
public:
    explicit IRQMSQLiteHelper(QObject *parent = nullptr);

    static sqlite3_stmt *prepare(sqlite3 *db, QString query, QVariantList bindings=QVariantList());
    static int bindValue(sqlite3_stmt *stmt, int pos, QVariant value);
    static QVariantMap fetchRow(sqlite3_stmt *stmt);

signals:

public slots:
};

#endif // IRQMSQLITEHELPER_H
