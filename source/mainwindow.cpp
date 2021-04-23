#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QDir"
#include "QDialog"
#include "QMessageBox"
#include "QFile"
#include "QFileInfo"
#include "QTextStream"
#include "unistd.h"
#include "QByteArray"
#include "crc16.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(), tr("STM32 Bin (*.bin)"));

    ui->label->setText(fileName);

    qint64 size = QFile(ui->label->text()).size();

    ui->statusBar->showMessage("Source File Size " + QString::number(size) + " Bytes");
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,"About Cheksum Utility","Append CRC16 checksum at the end of file\n\n"
                                    "Copyright (c) 2021 (MIT) Ing. Salvatore Cerami\ndev.salvatore.cerami@gmail.com\n"
                                    "https://github.com/sc-develop\ngit.sc.develop@gmail.com\n");
}

void MainWindow::on_actionOpen_triggered()
{
    on_pushButton_2_clicked();
}

void MainWindow::on_pushButton_clicked()
{
    int crcsize = 2; // 2 bytes crc size

    QFile file(ui->label->text());

    qint64 filesize = file.size();

    qint64 fillbytes = ui->lineEdit->text().toInt() - filesize - crcsize;

    if (fillbytes<0)
    {
        QMessageBox::warning(this,"Attenzione","La dimensione del file destinazione non può essere inferione a: " + QString::number(filesize + crcsize) + " Bytes");

        return;
    }

    QString path = QFileInfo(file.fileName()).absolutePath() + "/";

    QString filename = path + ui->lineEdit_Prefix->text().trimmed() + ui->lineEdit_Major->text().trimmed() + ui->lineEdit_Minor->text().trimmed() + ".bin";

    if (file.fileName()==filename) // se i file sorgente e destinazione coincidono (hanno lo stesso filepath)
    {
        QMessageBox::warning(this,"Attenzione", "Operazione abortita: file destinazione e sorgente coincidono.");
        return;
    }

    if (QFile::exists(filename))
    {
        if (QMessageBox::question(this,"Attenzione", "Sovrascrivere il file " + filename +"?")==QMessageBox::No)
        {
            return;
        }

        if (!file.remove(filename))
        {
            QMessageBox::critical(this,"Errore", "Impossible sovrascrivere il file " + filename);
            return;
        }
    }

    if (!file.copy(filename))
    {
        QMessageBox::critical(this,"Errore", "Impossible copiare il file " + file.fileName() + " in " + filename);
        return;
    }

    file.setFileName(filename);

    if (file.open(QIODevice::Append))
    {
        QDataStream out(&file);

        qint64 max = fillbytes;

        while (fillbytes--)
        {
           out << (uint8_t)0xFF;

           int pos = (max-fillbytes)/max * 100;

           ui->progressBar->setValue(pos);

           // usleep(10000);
        }

        file.close();

        if (file.open(QFile::ReadOnly))
        {
           QByteArray buff = file.readAll();

           // calc file crc *******************************************************************************

           int blen = buff.length();
           int flen = file.size();

           quint16 crc = qChecksum(buff.data(),buff.length(),Qt::ChecksumItuV41);

           crc16Init();

           for (int n=0; n<blen;n++)
           {
              uint8_t c = (uint8_t) buff.at(n);
              crc16Add(c);
           }

           crc = crcGet();

           QString hexcrc = QString("%1").arg(crc,4,16,QChar('0')).toLocal8Bit().toUpper();

           ui->lineEdit_crccalc->setText(hexcrc);

           file.close();

           // write crc to end of file *********************************************************************

           if (file.open(QIODevice::Append))
           {
             char crch = (char) (crc >> 8);
             char crcl = (char) (crc >> 0);

             file.write(&crch,1);
             file.write(&crcl,1);

             file.close();
           }
        }

        QMessageBox::information(this,"Information", "Operazione Completata");

        filesize = QFile(filename).size();

        if (file.open(QFile::ReadOnly))
        {
           QByteArray buff = file.readAll();

           ushort crch = buff[buff.length()-2] & 0x00FF;
           ushort crcl = buff[buff.length()-1] & 0x00FF;

           QString h = QString("%1").arg(crch,2,16,QChar('0')).toLocal8Bit().toUpper();
           QString l = QString("%1").arg(crcl,2,16,QChar('0')).toLocal8Bit().toUpper();

           ui->lineEdit_crcread->setText(h+l);

           file.close();
        }

        ui->lineEdit_4->setText(QString::number(filesize));
   }
   else
   {
     QMessageBox::critical(this,"Error","Impossibile aprire/creare il file: " + ui->label->text());
   }
}

void MainWindow::on_actionGenerate_triggered()
{
    on_pushButton_clicked();
}
