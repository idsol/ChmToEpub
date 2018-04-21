#include "application.h"
#include <QtCore>
#include "EpubWriter.h"

Application::Application(int argc, char *argv[])
    : QObject()
    , _verbose(false)
    , _forceOverwrite(false)
{
    QString arg;
    for (int i = 1; i < argc; ++i) {
        arg = QString::fromAscii(argv[i]);
        if (arg.toLower() == "--verbose" || arg.toLower() == "-v") {
            _verbose = true;
        }
        else if (arg.toLower() == "--overwrite" || arg.toLower() == "-o") {
            _forceOverwrite = true;
        }
        else {
            if (_inputFilename.isEmpty()) {
                _inputFilename = arg;
            } else {
                _outputFilename = arg;
            }
        }
    }

    // fill _outputFilename from _inputFilename if it's empty
    if (!_inputFilename.isEmpty() && _outputFilename.isEmpty()) {
        QFileInfo ifi(_inputFilename);
        QString ofn = ifi.path() + "/" + ifi.completeBaseName() + ".epub";
        _outputFilename = ofn;
    }

    // if it's output dir, complete it to filename
    QString ofnLowered = _outputFilename.toLower();
    bool isDir = !ofnLowered.endsWith(".epub")
                 || ofnLowered.endsWith("/")
                 || ofnLowered.endsWith("\\");
    if (isDir) {
        QFileInfo ifi(_inputFilename);
        _outputFilename = _outputFilename + "/" + ifi.completeBaseName() + ".epub";
    }
}

int Application::exec()
{
    if (!isValid()) {
        printUsage();
        return -1;
    }

    QTextStream out(stdout);
    QTextStream err(stderr);

    // show error & abort if chm file does not exist
    if (!QFileInfo(_inputFilename).exists()) {
        err << QObject::tr("Input file `%1` does not exist.").arg(_inputFilename) << endl;
        return -1;
    }

    // make output dir ready
    QDir outputDir = QDir(QFileInfo(_outputFilename).path());
    if (!outputDir.exists()) {
        if (!outputDir.mkpath(".")) {
            err << QObject::tr("Cannot create directory: `%1`").arg(outputDir.path()) << endl;
        }
    }

    // prompt for overwriting
    if (QFileInfo(_outputFilename).exists()) {
        if (_forceOverwrite) {
            QFile(_outputFilename).remove();
        } else {
            out << QObject::tr("Output file `%1` exists, skipped. Use -f switch if you mean to overwrite it.")
                    .arg(_outputFilename) << endl;
            return -1;
        }
    }

    EpubWriter writer(_inputFilename, _outputFilename);
    QObject::connect(&writer, SIGNAL(writeStarted(int)), this, SLOT(epubWriter_writeStarted(int)));
    if (_verbose)
        QObject::connect(&writer, SIGNAL(writeProgress(int,QString)), this, SLOT(epubWriter_writeProgress(int,QString)));
    QObject::connect(&writer, SIGNAL(writeFinished(bool)), this, SLOT(epubWriter_writeFinished(bool)));
    writer.execute();

    return 0;
}

void Application::epubWriter_writeStarted(int maximum)
{
    QTextStream out(stdout);
    EpubWriter* writer = qobject_cast<EpubWriter*>(sender());
    out << tr("Converting chm file `%1` to `%2`")
            .arg(writer->chmFilename()).arg(writer->epubFilename()) << endl;
}

void Application::epubWriter_writeProgress(int progress, const QString& text)
{
    QTextStream out(stdout);
    out << text << endl;
}

void Application::epubWriter_writeFinished(bool ok)
{
    EpubWriter* writer = qobject_cast<EpubWriter*>(sender());
    if (ok) {
        QTextStream out(stdout);
        out << QObject::tr("Done with `%1` in %2 milli-seconds.")
                .arg(writer->chmFilename()).arg(writer->executionDuration()) << endl;
    }
    else {
        QTextStream err(stderr);
        QString errorText = writer->errorText();
        err << errorText << endl;
    }
}

bool Application::isValid() const
{
    return !_inputFilename.isEmpty() && !_outputFilename.isEmpty();
}

void Application::printUsage()
{
    QTextStream out(stdout);
    out << "ChmToEpub 1.01 by sol." << endl << endl;
    out << "Usage:" << endl;
    out << "  ChmToEpub <in_file> [out_file|out_dir] [-v] [-f]" << endl;
    out << endl;
    out << "Arguments:" << endl;
    out << "  <in_file> = Input file, i.e. CHM file." << endl;
    out << "  [out_file|out_dir] = Output file, i.e. EPUB file. Or, output dir." << endl;
    out << "  -v | --verbose = Show in-process details." << endl;
    out << "  -f | --overwrite = Overwrite output file if it exists." << endl;
}
