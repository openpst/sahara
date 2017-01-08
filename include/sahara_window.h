/**
* LICENSE PLACEHOLDER
*
* @file sahara_window.h
* @class SaharaWindow
* @package OpenPST
* @brief Sahara GUI interface class definitions
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#ifndef _GUI_SAHARA_WINDOW_H
#define _GUI_SAHARA_WINDOW_H

#include <QMainWindow>
#include <QVariant>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include "ui_sahara_window.h"
#include "qualcomm/sahara_serial.h"
#include "qualcomm/sahara.h"
#include "util/hexdump.h"
#include "util/sleep.h"
#include "util/endian.h"
#include "about_dialog.h"
#include "task_runner_window.h"
#include "task/sahara_memory_read_task.h"
#include "task/sahara_image_transfer_task.h"

using namespace OpenPST::GUI;

using OpenPST::QC::SaharaSerial;
using OpenPST::QC::SaharaState;
using serial::PortInfo;

namespace Ui {
	class SaharaWindow;
}

namespace OpenPST {
	namespace GUI {
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
				PortInfo currentPort;
				SaharaState deviceState;
				int taskCount = 0;
				AboutDialog aboutDialog;
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

				void showAboutDialog();

				void cancelCurrentTask();
				void cancelAllTasks();
				void onTaskStarted();
				void onTaskComplete();
				void onTaskAborted();					
				void onTaskError(QString msg);
				void onTaskLog(QString msg);

			protected:
				void updateDeviceState();
				void closeEvent(QCloseEvent *event);
				void addTask(Task* task);

		};
	}
}
#endif // _GUI_SAHARA_WINDOW_H
