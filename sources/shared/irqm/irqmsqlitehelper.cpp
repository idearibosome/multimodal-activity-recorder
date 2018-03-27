#include "irqmsqlitehelper.h"

#include "sqlite3.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
IRQMSQLiteHelper::IRQMSQLiteHelper(QObject *parent) : QObject(parent) {

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
sqlite3_stmt *IRQMSQLiteHelper::prepare(sqlite3 *db, QString query, QVariantList bindings) {
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, query.toUtf8().constData(), -1, &stmt, NULL);

    for (int i=0; i<bindings.size(); i++) {
        bindValue(stmt, i+1, bindings.at(i));
    }

    return stmt;
}
//---------------------------------------------------------------------------
int IRQMSQLiteHelper::bindValue(sqlite3_stmt *stmt, int pos, QVariant value) {
    QVariant::Type type = value.type();

    if (!value.isValid() || value.isNull()) {
        return sqlite3_bind_null(stmt, pos);
    }

    switch (type) {
    case QMetaType::Bool:
    case QMetaType::Int:
    case QMetaType::Short:
    case QMetaType::UShort:
    case QMetaType::QChar:
    case QMetaType::Char:
    case QMetaType::SChar:
    case QMetaType::UChar:
        return sqlite3_bind_int(stmt, pos, value.toInt());

    case QMetaType::UInt:
    case QMetaType::Long:
    case QMetaType::LongLong:
    case QMetaType::ULong:
    case QMetaType::ULongLong:
        return sqlite3_bind_int64(stmt, pos, value.toLongLong());

    case QMetaType::Double:
    case QMetaType::Float:
        return sqlite3_bind_double(stmt, pos, value.toDouble());

    case QMetaType::QString: {
        const QString *string = static_cast<const QString *>(value.constData());
        return sqlite3_bind_text16(stmt, pos, string->utf16(), (string->size() * sizeof(QChar)), SQLITE_TRANSIENT);
    }

    case QMetaType::QByteArray: {
        const QByteArray *byteArray = static_cast<const QByteArray *>(value.constData());
        return sqlite3_bind_blob(stmt, pos, byteArray->constData(), byteArray->size(), SQLITE_TRANSIENT);
    }

    case QMetaType::Void:
        return sqlite3_bind_null(stmt, pos);

    default: {
        QString string = value.toString();
        return sqlite3_bind_text16(stmt, pos, string.utf16(), (string.size() * sizeof(QChar)), SQLITE_TRANSIENT);
    }
    }

    return SQLITE_MISUSE;
}
//---------------------------------------------------------------------------
QVariantMap IRQMSQLiteHelper::fetchRow(sqlite3_stmt *stmt) {
    QVariantMap row;

    int columns = sqlite3_column_count(stmt);

    for (int i=0; i<columns; i++) {
        QString key = QString((const char *)sqlite3_column_name(stmt, i));
        int columnType = sqlite3_column_type(stmt, i);

        if (columnType == SQLITE_INTEGER) {
            row.insert(key, sqlite3_column_int(stmt, i));
        }
        else if (columnType == SQLITE_FLOAT) {
            row.insert(key, sqlite3_column_double(stmt, i));
        }
        else if (columnType == SQLITE_BLOB) {
            row.insert(key, QByteArray((const char *)sqlite3_column_blob(stmt, i), sqlite3_column_bytes(stmt, i)));
        }
        else if (columnType == SQLITE_NULL) {
            row.insert(key, QVariant());
        }
        else {
            row.insert(key, QString((const QChar *)sqlite3_column_text16(stmt, i), (sqlite3_column_bytes16(stmt, i) / sizeof(QChar))));
        }
    }

    return row;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
