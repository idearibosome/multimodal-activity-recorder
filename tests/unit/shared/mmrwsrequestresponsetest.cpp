#include "mmrwsrequestresponsetest.h"

#include "shared/mmrwsrequest.h"
#include "shared/mmrwsresponse.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MMRWSRequestResponseTest::initTestCase() {

}
//------------------------------------------------------------------------------
void MMRWSRequestResponseTest::cleanupTestCase() {

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MMRWSRequestResponseTest::testRequestSerialization() {
    QString requestType = "test";
    QVariantMap requestParams;

    requestParams.insert("test1", QVariant(10));
    requestParams.insert("test2", QVariant(20.5));
    requestParams.insert("test3", "haha");

    MMRWSRequest sendObject;
    sendObject.requestType = requestType;
    sendObject.requestParams = requestParams;

    QByteArray serialized = sendObject.toByteArray();

    MMRWSRequest receiveObject;
    receiveObject.loadFromByteArray(serialized);

    QCOMPARE(sendObject.requestType, requestType);
    QCOMPARE(receiveObject.requestType, sendObject.requestType);

    foreach (QString key, requestParams.keys()) {
        QVERIFY(requestParams.value(key).isValid());
        QVERIFY(requestParams.value(key) == sendObject.requestParams.value(key));
    }
    foreach (QString key, sendObject.requestParams.keys()) {
        QVERIFY(sendObject.requestParams.value(key).isValid());
        QVERIFY(requestParams.value(key) == sendObject.requestParams.value(key));
    }
    foreach (QString key, receiveObject.requestParams.keys()) {
        QVERIFY(receiveObject.requestParams.value(key).isValid());
        QVERIFY(sendObject.requestParams.value(key) == receiveObject.requestParams.value(key));
    }
}
//------------------------------------------------------------------------------
void MMRWSRequestResponseTest::testResponseSerialization() {
    QString requestType = "test";
    QVariantMap responseData;

    responseData.insert("test1", QVariant(10));
    responseData.insert("test2", QVariant(20.5));
    responseData.insert("test3", "haha");

    MMRWSResponse sendObject;
    sendObject.requestType = requestType;
    sendObject.responseData = responseData;

    QByteArray serialized = sendObject.toByteArray();

    MMRWSResponse receiveObject;
    receiveObject.loadFromByteArray(serialized);

    QCOMPARE(sendObject.requestType, requestType);
    QCOMPARE(receiveObject.requestType, sendObject.requestType);

    QVariantMap sendObjectResponseData = sendObject.responseData.toMap();
    QVariantMap receiveObjectResponseData = receiveObject.responseData.toMap();

    foreach (QString key, responseData.keys()) {
        QVERIFY(responseData.value(key).isValid());
        QVERIFY(responseData.value(key) == sendObjectResponseData.value(key));
    }
    foreach (QString key, sendObjectResponseData.keys()) {
        QVERIFY(sendObjectResponseData.value(key).isValid());
        QVERIFY(responseData.value(key) == sendObjectResponseData.value(key));
    }
    foreach (QString key, receiveObjectResponseData.keys()) {
        QVERIFY(receiveObjectResponseData.value(key).isValid());
        QVERIFY(sendObjectResponseData.value(key) == receiveObjectResponseData.value(key));
    }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
