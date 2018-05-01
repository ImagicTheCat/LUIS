
#include <QApplication>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <iostream>
#include <sstream>
#include "SignalHandler.hpp"
#include "../src/tools.hpp"

int main(int argc, char **argv)
{
  if(argc >= 2){
    std::string data(hex2buf(std::string(argv[1])));
    // parse data
    std::vector<std::string> lines;
    split(data, "\r\n", lines);

    size_t nids = 0;
    std::string service;
    std::string client;
    std::string title;
    std::string contract;
    std::vector<std::string> identities;

    // read n-identities
    if(lines.size() > 0){
      std::stringstream ss;
      ss.str(lines[0]);
      ss >> nids;
    }

    // read identities
    for(size_t i = 1; i <= nids; i++){
      if(i < lines.size())
        identities.push_back(lines[i]);
    }

    // read service, client, title
    if(lines.size() > nids+1)
      service = lines[nids+1];
    if(lines.size() > nids+2)
      client = lines[nids+2];
    if(lines.size() > nids+3)
      title = lines[nids+3];

    // read contract
    for(size_t i = nids+4; i < lines.size(); i++)
      contract += lines[i]+"\r\n";

    QApplication app(argc, argv);

    QWidget window;
    QVBoxLayout w_layout;

    // form
    QGroupBox form_box("LUIS contract");
    QFormLayout form_layout;

    QLabel wtitle(QString::fromStdString(title));
    wtitle.setStyleSheet("font-weight: bold;");

    // service
    QLabel wservice;
    std::vector<std::string> service_args;
    split(service, " ", service_args);
    if(service_args.size() >= 2){
      service = "";
      for(size_t i = 2; i < service_args.size(); i++)
        service += service_args[i]+(i == service_args.size()-1 ? "" : " ");

      bool registered = (service_args[1] == "registered");
      wservice.setText(QString::fromStdString((registered ? " ":"UNREGISTERED ")+service_args[0]+" ("+service+")"));
      if(!registered)
        wservice.setStyleSheet("color: red;");
    }

    // client
    QLabel wclient;
    std::vector<std::string> client_args;
    split(client, " ", client_args);
    if(client_args.size() >= 2){
      client = "";
      for(size_t i = 2; i < client_args.size(); i++)
        client += client_args[i]+(i == client_args.size()-1 ? "" : " ");

      bool registered = (client_args[1] == "registered");
      wclient.setText(QString::fromStdString((registered ? " ":"UNREGISTERED ")+client_args[0]+" ("+client+")"));
      if(!registered)
        wclient.setStyleSheet("color: red;");
    }

    QComboBox widentity;
    QLineEdit wpassword;
    wpassword.setEchoMode(QLineEdit::Password);
    QTextEdit wcontract(QString::fromStdString(contract));
    wcontract.setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    wcontract.setText(QString::fromStdString(contract));

    for(size_t i = 0; i < identities.size(); i++)
      widentity.addItem(QString::fromStdString(identities[i]));

    form_layout.addRow(new QLabel("title"), &wtitle);
    form_layout.addRow(new QLabel("service"), &wservice);
    form_layout.addRow(new QLabel("client"), &wclient);
    form_layout.addRow(new QLabel("identity"), &widentity);
    form_layout.addRow(new QLabel("password"), &wpassword);
    form_layout.addRow(new QLabel("contract"), &wcontract);

    form_box.setLayout(&form_layout);
    w_layout.addWidget(&form_box);

    // final
    QGroupBox f_box("Action");
    QHBoxLayout f_layout;

    QPushButton wblacklist("blacklist");
    QComboBox wblacklist_type;
    wblacklist_type.addItem("both");
    wblacklist_type.addItem("service");
    wblacklist_type.addItem("client");
    QPushButton wcancel("cancel");
    QPushButton waccept("accept");

    f_layout.addWidget(&wblacklist);
    f_layout.addWidget(&wblacklist_type);
    f_layout.addWidget(&wcancel);
    f_layout.addWidget(&waccept);
    f_box.setLayout(&f_layout);
    w_layout.addWidget(&f_box);

    window.setLayout(&w_layout);


    // bind signals
    SignalHandler sh(identities, wblacklist_type, widentity, wpassword);
    QObject::connect(&wblacklist, SIGNAL(clicked()), &sh, SLOT(blacklist_clicked()));
    QObject::connect(&wcancel, SIGNAL(clicked()), &sh, SLOT(cancel_clicked()));
    QObject::connect(&waccept, SIGNAL(clicked()), &sh, SLOT(accept_clicked()));
    QObject::connect(&widentity, SIGNAL(currentIndexChanged(int)), &sh, SLOT(identity_changed(int)));

    // init
    sh.identity_changed(0);
    window.show();

    return app.exec();
  }
  else
    return 1;
}
