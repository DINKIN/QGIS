/***************************************************************************
                             qgsprocessingalgorithmdialogbase.cpp
                             ------------------------------------
    Date                 : November 2017
    Copyright            : (C) 2017 Nyall Dawson
    Email                : nyall dot dawson at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsprocessingalgorithmdialogbase.h"
#include "qgssettings.h"
#include "qgshelp.h"
#include "qgsmessagebar.h"
#include "processing/qgsprocessingalgorithm.h"
#include "processing/qgsprocessingprovider.h"
#include <QToolButton>
#include <QDesktopServices>

///@cond NOT_STABLE

QgsProcessingAlgorithmDialogFeedback::QgsProcessingAlgorithmDialogFeedback( QgsProcessingAlgorithmDialogBase *dialog )
  : QgsProcessingFeedback()
  , mDialog( dialog )
{
}

void QgsProcessingAlgorithmDialogFeedback::setProgressText( const QString &text )
{
  mDialog->setProgressText( text );
}

void QgsProcessingAlgorithmDialogFeedback::reportError( const QString &error )
{
  mDialog->reportError( error );
}

void QgsProcessingAlgorithmDialogFeedback::pushInfo( const QString &info )
{
  mDialog->pushInfo( info );
}

void QgsProcessingAlgorithmDialogFeedback::pushCommandInfo( const QString &info )
{
  mDialog->pushCommandInfo( info );
}

void QgsProcessingAlgorithmDialogFeedback::pushDebugInfo( const QString &info )
{
  mDialog->pushDebugInfo( info );
}

void QgsProcessingAlgorithmDialogFeedback::pushConsoleInfo( const QString &info )
{
  mDialog->pushConsoleInfo( info );
}

//
// QgsProcessingAlgorithmDialogBase
//

QgsProcessingAlgorithmDialogBase::QgsProcessingAlgorithmDialogBase( QWidget *parent, Qt::WindowFlags flags )
  : QDialog( parent, flags )
{
  setupUi( this );

  //don't collapse parameters panel
  splitter->setCollapsible( 0, false );

  // add collapse button to splitter
  QSplitterHandle *splitterHandle = splitter->handle( 1 );
  QVBoxLayout *handleLayout = new QVBoxLayout();
  handleLayout->setContentsMargins( 0, 0, 0, 0 );
  mButtonCollapse = new QToolButton( splitterHandle );
  mButtonCollapse->setAutoRaise( true );
  mButtonCollapse->setFixedSize( 12, 12 );
  mButtonCollapse->setCursor( Qt::ArrowCursor );
  handleLayout->addWidget( mButtonCollapse );
  handleLayout->addStretch();
  splitterHandle->setLayout( handleLayout );

  QgsSettings settings;
  restoreGeometry( settings.value( QStringLiteral( "/Processing/dialogBase" ) ).toByteArray() );
  splitter->restoreState( settings.value( QStringLiteral( "/Processing/dialogBaseSplitter" ), QByteArray() ).toByteArray() );
  mSplitterState = splitter->saveState();
  splitterChanged( 0, 0 );

  connect( mButtonBox, &QDialogButtonBox::rejected, this, &QgsProcessingAlgorithmDialogBase::reject );
  connect( mButtonBox, &QDialogButtonBox::accepted, this, &QgsProcessingAlgorithmDialogBase::accept );

  // Rename OK button to Run
  mButtonRun = mButtonBox->button( QDialogButtonBox::Ok );
  mButtonRun->setText( tr( "Run" ) );

  buttonCancel->setEnabled( false );
  mButtonClose = mButtonBox->button( QDialogButtonBox::Close );

  connect( mButtonBox, &QDialogButtonBox::helpRequested, this, &QgsProcessingAlgorithmDialogBase::openHelp );
  connect( mButtonCollapse, &QToolButton::clicked, this, &QgsProcessingAlgorithmDialogBase::toggleCollapsed );
  connect( splitter, &QSplitter::splitterMoved, this, &QgsProcessingAlgorithmDialogBase::splitterChanged );

  mMessageBar = new QgsMessageBar();
  mMessageBar->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
  verticalLayout->insertWidget( 0,  mMessageBar );
}

void QgsProcessingAlgorithmDialogBase::setAlgorithm( QgsProcessingAlgorithm *algorithm )
{
  mAlgorithm = algorithm;
  setWindowTitle( mAlgorithm->displayName() );

  QString algHelp = formatHelp( algorithm );
  if ( algHelp.isEmpty() )
    textShortHelp->hide();
  else
  {
    textShortHelp->document()->setDefaultStyleSheet( QStringLiteral( ".summary { margin-left: 10px; margin-right: 10px; }\n"
        "h2 { color: #555555; padding-bottom: 15px; }\n"
        "a { text - decoration: none; color: #3498db; font-weight: bold; }\n"
        "p { color: #666666; }\n"
        "b { color: #333333; }\n"
        "dl dd { margin - bottom: 5px; }" ) );
    textShortHelp->setHtml( algHelp );
    connect( textShortHelp, &QTextBrowser::anchorClicked, this, &QgsProcessingAlgorithmDialogBase::linkClicked );
  }
}

QgsProcessingAlgorithm *QgsProcessingAlgorithmDialogBase::algorithm()
{
  return mAlgorithm;
}

void QgsProcessingAlgorithmDialogBase::setMainWidget( QWidget *widget )
{
  if ( mMainWidget )
  {
    mMainWidget->deleteLater();
  }

  mMainWidget = widget;
  mTabWidget->widget( 0 )->layout()->addWidget( mMainWidget );
}

QWidget *QgsProcessingAlgorithmDialogBase::mainWidget()
{
  return mMainWidget;
}

QVariantMap QgsProcessingAlgorithmDialogBase::getParameterValues() const
{
  return QVariantMap();
}

QgsProcessingFeedback *QgsProcessingAlgorithmDialogBase::createFeedback()
{
  auto feedback = qgis::make_unique< QgsProcessingAlgorithmDialogFeedback >( this );
  connect( feedback.get(), &QgsProcessingFeedback::progressChanged, this, &QgsProcessingAlgorithmDialogBase::setPercentage );
  connect( buttonCancel, &QPushButton::clicked, feedback.get(), &QgsProcessingFeedback::cancel );
  return feedback.release();
}

QDialogButtonBox *QgsProcessingAlgorithmDialogBase::buttonBox()
{
  return mButtonBox;
}

QTabWidget *QgsProcessingAlgorithmDialogBase::tabWidget()
{
  return mTabWidget;
}

void QgsProcessingAlgorithmDialogBase::showLog()
{
  mTabWidget->setCurrentIndex( 1 );
}

void QgsProcessingAlgorithmDialogBase::closeEvent( QCloseEvent *e )
{
  saveWindowGeometry();
  QDialog::closeEvent( e );
}

QPushButton *QgsProcessingAlgorithmDialogBase::runButton()
{
  return mButtonRun;
}

QPushButton *QgsProcessingAlgorithmDialogBase::cancelButton()
{
  return buttonCancel;
}

void QgsProcessingAlgorithmDialogBase::clearProgress()
{
  progressBar->setMaximum( 0 );
}

void QgsProcessingAlgorithmDialogBase::setExecuted( bool executed )
{
  mExecuted = executed;
}

void QgsProcessingAlgorithmDialogBase::finished( bool, const QVariantMap &, QgsProcessingContext &, QgsProcessingFeedback * )
{

}

void QgsProcessingAlgorithmDialogBase::accept()
{
}

void QgsProcessingAlgorithmDialogBase::reject()
{
  saveWindowGeometry();
  QDialog::reject();
}

void QgsProcessingAlgorithmDialogBase::openHelp()
{
  QUrl algHelp = mAlgorithm->helpUrl();
  if ( algHelp.isEmpty() )
  {
    algHelp = QgsHelp::helpUrl( QStringLiteral( "processing_algs/%1/%2" ).arg( mAlgorithm->provider()->id(), mAlgorithm->id() ) );
  }

  if ( !algHelp.isEmpty() )
    QDesktopServices::openUrl( algHelp );
}

void QgsProcessingAlgorithmDialogBase::toggleCollapsed()
{
  if ( mHelpCollapsed )
  {
    splitter->restoreState( mSplitterState );
    mButtonCollapse->setArrowType( Qt::RightArrow );
  }
  else
  {
    mSplitterState = splitter->saveState();
    splitter->setSizes( QList<int>() << 1 << 0 );
    mButtonCollapse->setArrowType( Qt::LeftArrow );
  }
  mHelpCollapsed = !mHelpCollapsed;
}

void QgsProcessingAlgorithmDialogBase::splitterChanged( int, int )
{
  if ( splitter->sizes().at( 1 ) == 0 )
  {
    mHelpCollapsed = true;
    mButtonCollapse->setArrowType( Qt::LeftArrow );
  }
  else
  {
    mHelpCollapsed = false;
    mButtonCollapse->setArrowType( Qt::RightArrow );
  }
}

void QgsProcessingAlgorithmDialogBase::linkClicked( const QUrl &url )
{
  QDesktopServices::openUrl( url.toString() );
}

void QgsProcessingAlgorithmDialogBase::reportError( const QString &error )
{
  setInfo( error, true );
  resetGui();
  mTabWidget->setCurrentIndex( 1 );
}

void QgsProcessingAlgorithmDialogBase::pushInfo( const QString &info )
{
  setInfo( info );
}

void QgsProcessingAlgorithmDialogBase::pushCommandInfo( const QString &command )
{
  txtLog->append( QStringLiteral( "<code>%1<code>" ).arg( command.toHtmlEscaped() ) );
}

void QgsProcessingAlgorithmDialogBase::pushDebugInfo( const QString &message )
{
  txtLog->append( QStringLiteral( "<span style=\"color:blue\">%1</span>" ).arg( message.toHtmlEscaped() ) );
}

void QgsProcessingAlgorithmDialogBase::pushConsoleInfo( const QString &info )
{
  txtLog->append( QStringLiteral( "<code><span style=\"color:blue\">%1</darkgray></code>" ).arg( info.toHtmlEscaped() ) );
}

void QgsProcessingAlgorithmDialogBase::setPercentage( double percent )
{
  // delay setting maximum progress value until we know algorithm reports progress
  if ( progressBar->maximum() == 0 )
    progressBar->setMaximum( 100 );
  progressBar->setValue( percent );
}

void QgsProcessingAlgorithmDialogBase::setProgressText( const QString &text )
{
  lblProgress->setText( text );
  setInfo( text, false );
}

QString QgsProcessingAlgorithmDialogBase::formatHelp( QgsProcessingAlgorithm *algorithm )
{
  QString text = algorithm->shortHelpString();
  if ( !text.isEmpty() )
  {
    QStringList paragraphs = text.split( '\n' );
    QString help;
    for ( const QString &paragraph : paragraphs )
    {
      help += QStringLiteral( "<p>%1</p>" ).arg( paragraph );
    }
    return QStringLiteral( "<h2>%1</h2>%2" ).arg( algorithm->displayName(), help );
  }
  else
    return QString();
}

void QgsProcessingAlgorithmDialogBase::saveWindowGeometry()
{

}

void QgsProcessingAlgorithmDialogBase::resetGui()
{
  lblProgress->clear();
  progressBar->setMaximum( 100 );
  progressBar->setValue( 0 );
  mButtonRun->setEnabled( true );
  mButtonClose->setEnabled( true );
}

QgsMessageBar *QgsProcessingAlgorithmDialogBase::messageBar()
{
  return mMessageBar;
}

void QgsProcessingAlgorithmDialogBase::hideShortHelp()
{
  textShortHelp->setVisible( false );
}

void QgsProcessingAlgorithmDialogBase::setInfo( const QString &message, bool isError, bool escapeHtml )
{
  if ( isError )
    txtLog->append( QStringLiteral( "<span style=\"color:red\">%1</span><br />" ).arg( message ) );
  else if ( escapeHtml )
    txtLog->append( message.toHtmlEscaped() );
  else
    txtLog->append( message );
}

///@endcond
