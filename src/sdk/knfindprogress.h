/*
 * Copyright (C) Kreogist Dev Team
 *
 * You can redistribute this software and/or modify it under the
 * terms of the HARERU Software License; either version 1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * license file for more details.
 */
#ifndef KNFINDPROGRESS_H
#define KNFINDPROGRESS_H

#include <QDialog>

class QLabel;
class QProgressBar;
class KNFindProgress : public QDialog
{
    Q_OBJECT
public:
    explicit KNFindProgress(QWidget *parent = nullptr);

signals:
    void quitSearch();

public slots:
    void setMaxCount(int count);
    void setFilePath(int id, const QString &path);
    void engineClose();

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void retranslate();

private:
    QProgressBar *m_progress;
    QPushButton *m_cancel;
    QLabel *m_filePath;
    bool m_closeFlag;
};

#endif // KNFINDPROGRESS_H
