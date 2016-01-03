// This file is part of RSS Guard.
//
// Copyright (C) 2011-2015 by Martin Rotter <rotter.martinos@gmail.com>
//
// RSS Guard is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RSS Guard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RSS Guard. If not, see <http://www.gnu.org/licenses/>.

#include "gui/discoverfeedsbutton.h"

#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "gui/dialogs/formmain.h"
#include "gui/tabwidget.h"
#include "gui/feedmessageviewer.h"
#include "gui/feedsview.h"
#include "core/feedsmodel.h"
#include "services/abstract/serviceroot.h"

#include <QVariant>


DiscoverFeedsButton::DiscoverFeedsButton(QWidget *parent) : QToolButton(parent), m_addresses(QStringList()) {
  setEnabled(false);
  setIcon(qApp->icons()->fromTheme(QSL("folder-feed")));
  setPopupMode(QToolButton::InstantPopup);
}

DiscoverFeedsButton::~DiscoverFeedsButton() {
}

void DiscoverFeedsButton::clearFeedAddresses() {
  setFeedAddresses(QStringList());
}

void DiscoverFeedsButton::setFeedAddresses(const QStringList &addresses) {
  setEnabled(!addresses.isEmpty());
  setToolTip(addresses.isEmpty() ?
               tr("This website does not contain any feeds.") :
               tr("Click me to add feeds from this website.\nThis website contains %n feed(s).", 0, addresses.size()));

  if (menu() == NULL) {
    // Initialize the menu.
    setMenu(new QMenu(this));
    connect(menu(), SIGNAL(triggered(QAction*)), this, SLOT(linkTriggered(QAction*)));
    connect(menu(), SIGNAL(aboutToShow()), this, SLOT(fillMenu()));
  }

  menu()->hide();
  m_addresses = addresses;
}

void DiscoverFeedsButton::linkTriggered(QAction *action) {
  QString url = action->property("url").toString();
  ServiceRoot *root = static_cast<ServiceRoot*>(action->property("root").value<void*>());

  if (root->supportsFeedAddingByUrl()) {
    root->addFeedByUrl(url);
  }
  else {
    qApp->showGuiMessage(tr("Not supported"),
                         tr("Give account does not support adding feeds."),
                         QSystemTrayIcon::Warning,
                         qApp->mainForm(), true);
  }
}

void DiscoverFeedsButton::fillMenu() {
  menu()->clear();

  foreach (ServiceRoot *root, qApp->mainForm()->tabWidget()->feedMessageViewer()->feedsView()->sourceModel()->serviceRoots()) {
    QMenu *root_menu = menu()->addMenu(root->icon(), root->title());

    foreach (const QString &url, m_addresses) {
      if (root->supportsFeedAddingByUrl()) {
        QAction *url_action = root_menu->addAction(root->icon(), url);

        url_action->setProperty("url", url);
        url_action->setProperty("root", QVariant::fromValue((void*) root));
      }
    }
  }
}
