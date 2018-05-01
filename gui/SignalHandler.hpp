#ifndef H_SIGNALHANDLER
#define H_SGINALHANDLER

#include <QApplication>
#include <QComboBox>
#include <QLineEdit>
#include <iostream>
#include "../src/tools.hpp"

class SignalHandler : public QObject{
  Q_OBJECT
  public:
    SignalHandler(std::vector<std::string> &_identities, QComboBox &_wblacklist_type, QComboBox &_widentity, QLineEdit &_wpassword);

  public slots:
    void blacklist_clicked();
    void cancel_clicked();
    void accept_clicked();
    void identity_changed(int index);

  private:
    std::vector<std::string> &identities;
    QComboBox &wblacklist_type, &widentity;
    QLineEdit &wpassword;
};

#endif
