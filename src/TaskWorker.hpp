#pragma once

#include <rtt/transports/corba/TaskContextServer.hpp>

#include <atomic>
#include <stdexcept>
#include <QMap>
#include <iterator>

#include <QObject>
#include <QBasicTimer>

class QTimerEvent;
class QBasicTimer;

class SignalSlotProvider : public QObject
{
    Q_OBJECT

public:
    SignalSlotProvider(QObject *parent = 0)
        : QObject(parent)
        , m_cancelled(false)
        , m_readInterval(100)
        , m_timer(new QBasicTimer)
    {
    }

public slots:
    void setReadInterval(unsigned int interval = 100)
    {
        m_readInterval = interval;
        if (m_timer->isActive()) {
            m_timer->stop();
            m_timer->start(m_readInterval, this);
        }
    }

    unsigned int getReadInterval()
    {
        return m_readInterval;
    }

    void start()
    {
        readInput();
        m_timer->start(m_readInterval, this);
    }

    void stop()
    {
        m_timer->stop();
    }

signals:
    void valueChanged(const QString &value);
    void dataAvailable();

protected:
    void timerEvent(QTimerEvent *)
    {
        readInput();
    }

    virtual void readInput() = 0;

private:
    std::atomic<bool> m_cancelled;
    unsigned int m_readInterval;
    QBasicTimer* m_timer;
};


template <typename T>
class TaskWorker : public SignalSlotProvider {
public:
    TaskWorker<T>(QObject* parent = 0)
        : SignalSlotProvider(parent)
        , m_reader(nullptr)
    {
    }

    void setReader(RTT::InputPort<T>* reader)
    {
        m_reader = reader;
    }

    void addObserver(const QString& name, std::function<void (T)> observer)
    {
        m_observers.insert(name, observer);
    }

    void removeObserver(const QString& name)
    {
        if (m_observers.has(name))
            m_observers.remove(name);
    }

    void removeObserver(std::function<void (T)> observer)
    {
        typename QMap<QString, std::function<void (T)> >::iterator iter;
        for (iter = m_observers.begin(); iter != m_observers.end(); ++iter) {
            if ((*iter).value() == observer)
                m_observers.remove((*iter).key());
        }
    }

protected:
    void virtual readInput()
    {
        T data;
        while(!m_observers.empty() && m_reader->read(data, false) == RTT::NewData) {
            for (auto f : m_observers)
                f(data);
            emit dataAvailable();
        }
    }

private:
    RTT::InputPort<T>* m_reader;
    QMap<QString, std::function<void (T)>> m_observers;
};
