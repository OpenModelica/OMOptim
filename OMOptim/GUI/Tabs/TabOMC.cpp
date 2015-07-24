// $Id$
        /**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL).
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

     @file tabOMC.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version
*/

#include "TabOMC.h"
#include "Plots/MOOptPlot.h"


        namespace Ui
{
    class TabOMC_Class;
}

TabOMC::TabOMC(Project *project_,QWidget *parent) :
        MOTabSimple(NULL,NULL), ui(new Ui::TabOMC_Class)
{

    ui->setupUi(this);

    project = project_;
    omc_version_ = "(version)";
    fontSize_ = 11;
    createMoshEdit();

    // sett start message
    cursor_.insertText( QString("OpenModelica ") + omc_version_ + "\n", textFormat_ );
    cursor_.insertText( "Copyright (c) OSMC 2002-2009\n\n", textFormat_ );
    cursor_.insertText( "To get help on using OMShell and OpenModelica, type \"help()\" and press enter.\n", textFormat_ );


    // create command compleation instance
    QString openmodelica( getenv( "OPENMODELICAHOME" ) );
    if( openmodelica.isEmpty() )
        QMessageBox::critical( 0, "OMShell Error", "Could not find environment variable OPENMODELICAHOME, command compleation will not work" );

    try
    {
        QString commandfile;
        if( openmodelica.endsWith("/") || openmodelica.endsWith( "\\") )
            commandfile = openmodelica + "bin/commands.xml";
        else
            commandfile = openmodelica + "/bin/commands.xml";

    //    commandcompletion_ = IAEX::CommandCompletion::instance( commandfile );
    }
    catch( std::exception &e )
    {
        QString msg = e.what();
        msg += "\nCould not create command completion class!";
        QMessageBox::warning( QApplication::activeWindow(), "Error", msg, "OK" );
    }

    // add function names for code completion
    /*currentFunction_ = -1;
    currentFunctionName_ = "";
    functionList_ = new QStringList();
    functionList_->push_back( "cd()" );
    functionList_->push_back( "cd(dir)" );
    functionList_->push_back( "clear()" );
    functionList_->push_back( "clearVariables()" );
    functionList_->push_back( "help()" );
    functionList_->push_back( "instantiateModel(modelname)" );
    functionList_->push_back( "list()" );
    functionList_->push_back( "list(modelname)" );
    functionList_->push_back( "loadFile(strFile)" );
    functionList_->push_back( "loadModel(name)" );
    functionList_->push_back( "listVariables()" );
    functionList_->push_back( "plot(vars)" );
    functionList_->push_back( "readFile(str)" );
    functionList_->push_back( "readSimulationResultSize(strFile)" );
    functionList_->push_back( "readSimulationResult(strFile, variables, size)" );
    functionList_->push_back( "runScript(strFile)" );
    functionList_->push_back( "saveModel(strFile, modelname)" );
    functionList_->push_back( "simulate(modelname, startTime=0, stopTime=1)" );
    functionList_->push_back( "system(str)" );
    functionList_->push_back( "timing(expr)" );
    functionList_->push_back( "typeOf(variable)" );*/

    // command stuff
    commandSignFormat_.setFontFamily( "Arial" );
    commandSignFormat_.setFontWeight( QFont::Bold );
    commandSignFormat_.setFontPointSize( fontSize_ );

    commands_ = new QStringList();
    currentCommand_ = -1;
    addCommandLine();
}


TabOMC::~TabOMC()
{
    delete ui;


    //delete commandcompletion_;

    //delete commands_;
    //delete functionList_;



}




void TabOMC::createMoshEdit()
{
    moshEdit_ = new MyTextEdit();
    ui->gridLayout->addWidget( moshEdit_ );
    cursor_ = moshEdit_->textCursor();

    moshEdit_->setReadOnly( false );
    moshEdit_->setFrameShadow( QFrame::Plain );
    moshEdit_->setFrameShape( QFrame::Panel );
    moshEdit_->setAutoFormatting( QTextEdit::AutoNone );

    moshEdit_->setSizePolicy( QSizePolicy(
            QSizePolicy::Expanding, QSizePolicy::Expanding ));
    moshEdit_->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    moshEdit_->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    moshEdit_->setContextMenuPolicy( Qt::NoContextMenu );

    // text settings
    moshEdit_->setFontFamily( "Courier New" );
    moshEdit_->setFontWeight( QFont::Normal );
    moshEdit_->setFontPointSize( fontSize_ );

    textFormat_ = moshEdit_->currentCharFormat();

    connect( moshEdit_, SIGNAL( returnPressed() ),
             this, SLOT( returnPressed() ));
    connect( moshEdit_, SIGNAL( insertNewline() ),
             this, SLOT( insertNewline() ));
    connect( moshEdit_, SIGNAL( prevCommand() ),
             this, SLOT( prevCommand() ));
    connect( moshEdit_, SIGNAL( nextCommand() ),
             this, SLOT( nextCommand() ));
    connect( moshEdit_, SIGNAL( goHome(bool) ),
             this, SLOT( goHome(bool) ));
    connect( moshEdit_, SIGNAL( codeCompletion(bool) ),
             this, SLOT( codeCompletion(bool) ));
    connect( moshEdit_, SIGNAL( codeNextField() ),
             this, SLOT( codeNextField() ));
}


// ******************************************************

MyTextEdit::MyTextEdit( QWidget* parent )
    : QTextEdit( parent )
{
    sameTab_ = false;
}

MyTextEdit::~MyTextEdit()
{
}

void MyTextEdit::sendKey( QKeyEvent *event )
{
    keyPressEvent( event );
}

void MyTextEdit::keyPressEvent(QKeyEvent *event)
{
    if( !insideCommandSign() )
    {
        switch( event->key() )
        {
        case Qt::Key_Backspace:
        case Qt::Key_Left:
            if( !startOfCommandSign() )
                QTextEdit::keyPressEvent( event );
            sameTab_ = false;
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            if( event->modifiers() == Qt::ShiftModifier )
                emit insertNewline();
            else
                emit returnPressed();
            sameTab_ = false;
            break;
        case Qt::Key_Up:
            emit prevCommand();
            sameTab_ = false;
            break;
        case Qt::Key_Down:
            emit nextCommand();
            sameTab_ = false;
            break;
        case Qt::Key_Home:
            if( event->modifiers() == Qt::ShiftModifier )
                emit goHome(true);
            else
                emit goHome(false);
            sameTab_ = false;
            break;
        case Qt::Key_Tab:
            {
                if( event->modifiers() == Qt::ControlModifier )
                {
                    emit codeNextField();
                    sameTab_ = false;
                }
                else
                {
                    emit codeCompletion( sameTab_ );
                    sameTab_ = true;
                }
            }
            break;
        default:
            QTextEdit::keyPressEvent( event );
            sameTab_ = false;
            break;
        }
    }
}

bool MyTextEdit::insideCommandSign()
{
    QTextBlock block = document()->findBlock( textCursor().position() );
    if( block.isValid() )
    {
        int signPos = block.text().indexOf( ">> ", 0, Qt::CaseInsensitive );
        int blockStartPos = block.position();
        int cursorPos = textCursor().position();
        if( blockStartPos <= cursorPos && cursorPos < (blockStartPos+3) && signPos == 0)
        {
            /*        cerr << "Inside Command Sign" << endl;
            cerr << "BlockStart: " << blockStartPos <<
                ", Cursor: " << cursorPos << endl << endl;*/


            return true;
        }
        else
            return false;
    }
    else
        //cerr << "Not a valid QTextBlock (insideCommandSign)" << endl;

    return true;
}

bool MyTextEdit::startOfCommandSign()
{
    QTextBlock block = document()->findBlock( textCursor().position() );
    if( block.isValid() )
    {
        int signPos = block.text().indexOf( ">> ", 0, Qt::CaseInsensitive );
        int blockStartPos = block.position();
        int cursorPos = textCursor().position();
        if( cursorPos == (blockStartPos+3) && signPos == 0 )
            return true;
        else
            return false;
    }
    else
        //cerr << "Not a valid QTextBlock (startOfCommandSign)" << endl;

    return true;
}

// ADDED 2006-01-30
// If the mimedata that should be insertet contain text,
// create a new mimedata object that only contains text, otherwise
// text format is insertet also - don't want that for inputcells.
void MyTextEdit::insertFromMimeData(const QMimeData *source)
{
    if( source->hasText() )
    {
        QMimeData *newSource = new QMimeData();
        newSource->setText( source->text() );
        QTextEdit::insertFromMimeData( newSource );
        delete newSource;
    }
    else
        QTextEdit::insertFromMimeData( source );
}
void TabOMC::addCommandLine()
{
    cursor_.movePosition( QTextCursor::End, QTextCursor::MoveAnchor );
    cursor_.insertBlock();
    cursor_.insertText( ">> " );
    cursor_.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
    cursor_.setPosition( cursor_.position()+2, QTextCursor::KeepAnchor );
    cursor_.mergeCharFormat( commandSignFormat_ );
    cursor_.clearSelection();
    cursor_.movePosition( QTextCursor::End, QTextCursor::MoveAnchor );
    moshEdit_->setTextCursor( cursor_ );

    // sett original text settings
    moshEdit_->setFontFamily( "Courier New" );
    textFormat_.setFontFamily( "Courier New" );
    moshEdit_->setFontWeight( QFont::Normal );
    textFormat_.setFontWeight( QFont::Normal );
    moshEdit_->setFontPointSize( fontSize_ );
    textFormat_.setFontPointSize( fontSize_ );

    // set correct scrollview
    moshEdit_->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}

void TabOMC::returnPressed()
{
    // find the last command sign
    cursor_.movePosition( QTextCursor::End, QTextCursor::MoveAnchor );
    QTextBlock block = moshEdit_->document()->findBlock( cursor_.position() );
    QString commandText;
    while( true )
    {
        if( block.isValid() )
        {
            commandText = block.text() + commandText;

            if( block.text().indexOf( ">> ", 0, Qt::CaseInsensitive ) == 0 )
            { // last command sign found, send command to OMC
                break;
            }
            else
            { // no command sign, look in previous text block
                block = block.previous();
                commandText = "\n" + commandText;
            }
        }
        else
        {
            //cerr << "Not a valid QTextBlock (returnPressed)" << endl;
            break;
        }
    }

    // strip command sign from commandText
    commandText.remove( 0, 3 );

    // save the last command
    commands_->append( commandText );
    currentCommand_ = -1;

    // remove any newline
    commandText.simplified();

    // if 'quit()' exit WinMosh
    if( commandText == "quit()" )
    {
        //exit();
        return;
    }

    // send command to OMC
    if(!project->moomc()->isStarted())
    {
        project->moomc()->startServer();
    }
    if( project->moomc()->isStarted() )
    {
        eval:
        // 2006-02-02 AF, Added try-catch
        try
        {
            project->moomc()->evalCommand(commandText );
        }
        catch( std::exception &e )
        {
            exceptionInEval(e);
            return;
        }

        // get result
        QString res = project->moomc()->getResult();

        if( res.isEmpty() )
            cursor_.insertText( "\n", textFormat_ );
        else
            cursor_.insertText( "\n" + res + "\n", textFormat_ );

        // get Error text
        try
        {
            QString getErrorString = "getErrorString()";
            project->moomc()->evalCommand(getErrorString);
        }
        catch( std::exception &e )
        {
            exceptionInEval(e);
            return;
        }
        QString error = project->moomc()->getResult();
        if( error.size() > 2 )
        {
            cursor_.insertText( error.mid( 1, error.size() - 2 ) );
            /*
            QTextCursor errorCursor = moshError_->textCursor();
            errorCursor.insertText( "\n" + error.mid( 0, error.size() - 1 ) );
            moshError_->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
            */
        }
    }
    else
    {
        if( project->moomc()->startServer() )
        {
            cursor_.insertText("[ERROR] No OMC server started - restarted OMC\n" );
            goto eval;
        }
        else
            cursor_.insertText("[ERROR] No OMC server started - unable to restart OMC\n" );


        /*
        QTextCursor cursor = moshError_->textCursor();

        if( startServer() )
        {
            cursor.insertText("[ERROR] No OMC serer started - restarted OMC\n" );
            goto eval;
        }
        else
            cursor.insertText("[ERROR] No OMC serer started - unable to restart OMC\n" );
        */
    }

    // add new command line
    addCommandLine();
}
void TabOMC::selectCommandLine()
{
    cursor_.movePosition( QTextCursor::End, QTextCursor::MoveAnchor );
    QTextBlock block = moshEdit_->document()->findBlock( cursor_.position() );
    while( true )
    {
        if( block.isValid() )
        {
            if( block.text().indexOf( ">> ", 0, Qt::CaseInsensitive ) == 0 )
            { // last command sign found, move cursor there
                cursor_.setPosition( block.position()+3, QTextCursor::KeepAnchor );
                break;
            }
            else
            { // no command sign, look in previous text block
                block = block.previous();
            }
        }
        else
        {
            //cout << "Not a valid QTextBlock (selectCommandLine)" << endl;
            break;
        }
    }

    moshEdit_->ensureCursorVisible();
}


void TabOMC::exceptionInEval(std::exception &e)
{
    // 2006-0-09 AF, try to reconnect to OMC first.
    try
    {
        //project->moomc()->getCommunicator()->closeConnection();
        //bool reconnected = project->moomc()->getCommunicator()->establishConnection();
        returnPressed();
    }
    catch( std::exception &e )
    {
        // unable to reconnect, ask if user want to restart omc.
        QString msg = QString( e.what() ) + "\n\nUnable to reconnect with OMC. Do you want to restart OMC?";
        int result = QMessageBox::critical( 0, tr("Communication Error with OMC"),
                                            msg,
                                            QMessageBox::Yes | QMessageBox::Default,
                                            QMessageBox::No );

        if( result == QMessageBox::Yes )
        {
            project->moomc()->stopServer();
            if( project->moomc()->startServer())
            {
                // 2006-03-14 AF, wait before trying to reconnect,
                // give OMC time to start up
                //SleeperThread::msleep( 1000 );

                //project->moomc()->delegate_->closeConnection();
                try
                {
                    //project->moomc()->getCommunicator()->establishConnection();
                    returnPressed();
                }
                catch( std::exception &e )
                {
                    QMessageBox::critical( 0, tr("Communication Error"),
                                           tr("<B>Unable to communication correctlly with OMC.</B>") );
                    //exit();
                }
            }
        }
    }
}

void TabOMC::insertNewline()
{
    cursor_ = moshEdit_->textCursor();
    cursor_.insertBlock();
    moshEdit_->setTextCursor( cursor_ );
    moshEdit_->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}

void TabOMC::prevCommand()
{
    if( commands_->size() > 0 )
    {
        if( currentCommand_ < 0 )
            currentCommand_ = commands_->size() - 1;
        else
        {
            if( currentCommand_ >= 1 )
                currentCommand_--;
            else
                currentCommand_ = 0;
        }

        // select all text in the last commandline
        selectCommandLine();
        cursor_.removeSelectedText();
        cursor_.insertText( commands_->at( currentCommand_ ));
        cursor_.movePosition( QTextCursor::EndOfBlock, QTextCursor::MoveAnchor );
        moshEdit_->setTextCursor( cursor_ );
    }

    moshEdit_->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}

void TabOMC::nextCommand()
{
    if( currentCommand_ >= 0 )
    {
        if( currentCommand_ == commands_->size()-1 )
        { // last command is currently displayed, clear the commandline
            currentCommand_ = -1;

            selectCommandLine();
            cursor_.removeSelectedText();
            cursor_.movePosition( QTextCursor::EndOfBlock, QTextCursor::MoveAnchor );
            moshEdit_->setTextCursor( cursor_ );
        }
        else
        {
            currentCommand_++;

            selectCommandLine();
            cursor_.removeSelectedText();
            cursor_.insertText( commands_->at( currentCommand_ ));
            cursor_.movePosition( QTextCursor::EndOfBlock, QTextCursor::MoveAnchor );
            moshEdit_->setTextCursor( cursor_ );
        }
    }
    else
    {
        // no erlier commands, clear the commandline..
        selectCommandLine();
        cursor_.removeSelectedText();
        cursor_.movePosition( QTextCursor::EndOfBlock, QTextCursor::MoveAnchor );
        moshEdit_->setTextCursor( cursor_ );
    }

    moshEdit_->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}


void TabOMC::cut()
{
    /*    if( moshEdit_->hasFocus() )
    {*/
    QKeyEvent* key = new QKeyEvent( QEvent::KeyPress, Qt::Key_X, Qt::ControlModifier, "x" );
    ((MyTextEdit*)moshEdit_)->sendKey( key );
    /*    }
    else if( moshError_->hasFocus() )
    {
        moshError_->copy();
    }*/
}

void TabOMC::copy()
{
    /*    if( moshEdit_->hasFocus() )
    {*/
    QKeyEvent* key = new QKeyEvent( QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier, "c" );
    ((MyTextEdit*)moshEdit_)->sendKey( key );
    /*    }
    else if( moshError_->hasFocus() )
    {
        moshError_->copy();
    }*/
}

void TabOMC::paste()
{
    QKeyEvent* key = new QKeyEvent( QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier, "v" );
    ((MyTextEdit*)moshEdit_)->sendKey( key );
}


void TabOMC::goHome( bool shift )
{
    QTextBlock block = moshEdit_->document()->findBlock( cursor_.position() );
    if( block.isValid() )
    {
        cursor_ = moshEdit_->textCursor();

        if( shift )
            cursor_.setPosition( block.position() + 3, QTextCursor::KeepAnchor );
        else
            cursor_.setPosition( block.position() + 3, QTextCursor::MoveAnchor );

        moshEdit_->setTextCursor( cursor_ );
    }
    //else
    //cout << "Not a valid QTextBlock (selectCommandLine)" << endl;
}

//void TabOMC::codeCompletion( bool same )
//{
//    cursor_ = moshEdit_->textCursor();
//    if( !same )
//    {
//        commandcompletion_->insertCommand( cursor_ );
//        moshEdit_->setTextCursor( cursor_ );
//    }
//    else
//    {
//        commandcompletion_->nextCommand( cursor_ );
//        moshEdit_->setTextCursor( cursor_ );
//    }
//
//    /*
//    cursor_ = moshEdit_->textCursor();
//
//    if( !same )
//    {
//        //find last word
//        cursor_.movePosition( QTextCursor::PreviousWord, QTextCursor::KeepAnchor );
//        currentFunctionName_ = cursor_.selectedText();
//        currentFunction_ = 0;
//    }
//
//
//    QStringList list = getFunctionNames( currentFunctionName_ );
//    if( list.isEmpty() )
//    {
//        if( currentFunctionName_ == "> " )
//            list = *functionList_;
//    }
//
//    if( !list.isEmpty() )
//    {
//        if( same )
//        {
//            if( currentFunction_ == list.size() - 1 )
//                currentFunction_ = 0;
//            else
//                currentFunction_++;
//        }
//
//        selectCommandLine();
//        cursor_.insertText( list.at( currentFunction_ ));
//        moshEdit_->setTextCursor( cursor_ );
//    }
//*/
//
//}

void TabOMC::codeNextField()
{
    cursor_ = moshEdit_->textCursor();
    //    commandcompletion_->nextField( cursor_ );
    moshEdit_->setTextCursor( cursor_ );
}


// ******************************************************
