
#include "SignalHandler.hpp"

SignalHandler::SignalHandler(std::vector<std::string> &_identities, QComboBox &_wblacklist_type, QComboBox &_widentity, QLineEdit &_wpassword)
  : identities(_identities), wblacklist_type(_wblacklist_type), widentity(_widentity), wpassword(_wpassword)
{
}

void SignalHandler::blacklist_clicked()
{
  std::cout << "blacklist " << wblacklist_type.currentText().toStdString() << std::flush;
  QApplication::quit();
}

void SignalHandler::cancel_clicked()
{
  QApplication::quit();
}

void SignalHandler::accept_clicked()
{
  std::cout << widentity.currentIndex() << " " << wpassword.text().toStdString() << std::flush;
  QApplication::quit();
}

void SignalHandler::identity_changed(int index)
{
  size_t i = index;
  if(i < identities.size()){
    std::vector<std::string> args;
    split(identities[i], " ", args);
    if(args.size() >= 2 && args[1] == "pass")
      wpassword.show();
    else
      wpassword.hide();
  }
}
