/**
*
* (c) Gassan Idriss <ghassani@gmail.com>
* 
* This file is part of sahara.
* 
* sahara is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* sahara is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with sahara. If not, see <http://www.gnu.org/licenses/>.
*
* @file sahara_window.h
* @class SaharaWindow
* @package openpst/sahara
* @brief Sahara GUI interface class definitions
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#pragma once

#include <QMainWindow>
#include <QVariant>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <vector>
#include "ui_sahara_window.h"
#include "qualcomm/sahara_serial.h"
#include "qualcomm/sahara.h"
#include "qualcomm/mbn_parser.h"
#include "qualcomm/sahara_xml_reader.h"
#include "util/hexdump.h"
#include "about_dialog.h"
#include "table_dialog.h"
#include "task/task_runner.h"
#include "task/sahara_memory_read_task.h"
#include "task/sahara_image_transfer_task.h"

using OpenPST::QC::SaharaSerial;
using OpenPST::QC::SaharaState;
using OpenPST::QC::SaharaSerialError;
using OpenPST::Serial::SerialError;
using OpenPST::QC::MbnParser;
using OpenPST::QC::Mbn;
using OpenPST::QC::MbnParserError;
using OpenPST::QC::SaharaXmlReader;
using OpenPST::QC::SaharaXmlReaderError;
using OpenPST::QC::SaharaXmlEntry;

namespace Ui {
	class SaharaWindow;
}

namespace OpenPST {
	namespace GUI {

		struct ResolvedSaharaXmlEntry {
			SaharaXmlEntry entry;
			QString sourceFile;
			QString path;
		};

		class SaharaWindow : public QMainWindow
		{
			Q_OBJECT

			enum SaharaMemoryDebugTableColumns{
				kSaharaMemoryDebugTableColumnName = 0,
				kSaharaMemoryDebugTableColumnFileName,
				kSaharaMemoryDebugTableColumnAddress,
				kSaharaMemoryDebugTableColumnSize,
			};
			
			private:
				Ui::SaharaWindow *ui; 
				TaskRunner taskRunner;
				volatile bool taskShouldCancel = false;				
				SaharaSerial port;
				serial::PortInfo currentPort;
				SaharaState deviceState;
				int taskCount = 0;
				AboutDialog aboutDialog;
				std::vector<ResolvedSaharaXmlEntry> images;
			public:
				explicit SaharaWindow(QWidget *parent = 0);
				~SaharaWindow();

			public slots:
				/**
				* @brief updatePortList
				*/
				void updatePortList();

				/**
				* @brief connectToPort
				*/
				void connectToPort();

				/**
				* @brief disconnectPort
				*/
				void disconnectPort();

				/**
				* @brief readHello
				*/
				void readHello();

				/**
				* @brief writeHello
				*/
				void writeHello(uint32_t overrideMode = NULL);

				/**
				* @brief switchMode
				*/
				void switchMode();

				/**
				* @brief sendClientCommand
				*/
				void sendClientCommand();

				/**
				* @brief sendReset
				*/
				void sendReset();

				/**
				* @brief browseForImage
				*/
				void browseForImage();

				/**
				* @brief sendImage
				*/
				void sendImage();

				/**
				* @brief checkImage
				*/
				void checkImage();

				/**
				* @brief browseForXml
				*/
				void browseForXml();

				/**
				* @brief checkXml
				*/
				void checkXml();

				/**
				* @brief sendDone
				*/
				void sendDone();

				/**
				* @brief memoryRead
				*/
				void memoryRead();

				/**
				* @brief debugMemoryRead
				*/
				void debugMemoryRead();

				/**
				* @brief showAboutDialog
				*/
				void showAboutDialog();

				/**
				* @brief cancelCurrentTask
				*/
				void cancelCurrentTask();

				/**
				* @brief cancelAllTasks
				*/
				void cancelAllTasks();

				/**
				* @brief onTaskStarted
				*/
				void onTaskStarted();

				/**
				* @brief onTaskComplete
				*/
				void onTaskComplete();

				/**
				* @brief onTaskAborted
				*/
				void onTaskAborted();	

				/**
				* @brief onTaskError
				*/				
				void onTaskError(QString msg);
				
				/**
				* @brief onTaskLog
				*/		
				void onTaskLog(QString msg);

			protected:
				/**
				* @brief updateDeviceState
				*/	
				void updateDeviceState();
				
				/**
				* @brief closeEvent
				*/	
				void closeEvent(QCloseEvent *event);
				
				/**
				* @brief addTask
				*/	
				void addTask(Task* task);

				/**
				* @brief parseSaharaXml
				*/	
				void parseSaharaXml(const QString& filePath);

		};
	}
}
