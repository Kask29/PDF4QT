//    Copyright (C) 2019 Jakub Melka
//
//    This file is part of PdfForQt.
//
//    PdfForQt is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    PdfForQt is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with PDFForQt.  If not, see <https://www.gnu.org/licenses/>.

#ifndef PDFCOMPILER_H
#define PDFCOMPILER_H

#include "pdfrenderer.h"
#include "pdfpainter.h"

#include <QCache>
#include <QFuture>
#include <QFutureWatcher>

namespace pdf
{
class PDFDrawWidgetProxy;

/// Asynchronous page compiler compiles pages asynchronously, and stores them in the
/// cache. Cache size can be set. This object is designed to cooperate with
/// draw widget proxy.
class PDFAsynchronousPageCompiler : public QObject
{
    Q_OBJECT

private:
    using BaseClass = QObject;

public:
    explicit PDFAsynchronousPageCompiler(PDFDrawWidgetProxy* proxy);

    /// Starts the engine. Call this function only if the engine
    /// is stopped.
    void start();

    /// Stops the engine and all underlying asynchronous tasks. Also
    /// clears the cache. Call this function only if engine is active.
    void stop();

    /// Resets the engine - calls stop and then calls start.
    void reset();

    /// Sets cache limit in bytes
    /// \param limit Cache limit [bytes]
    void setCacheLimit(int limit);

    enum class State
    {
        Inactive,
        Active,
        Stopping
    };

    /// Tries to retrieve precompiled page from the cache. If page is not found,
    /// then nullptr is returned (no exception is thrown). If \p compile is set to true,
    /// and page is not found, and compiler is active, then new asynchronous compile
    /// task is performed.
    /// \param pageIndex Index of page
    /// \param compile Compile the page, if it is not found in the cache
    const PDFPrecompiledPage* getCompiledPage(PDFInteger pageIndex, bool compile);

signals:
    void pageImageChanged(bool all, const std::vector<PDFInteger>& pages);
    void renderingError(PDFInteger pageIndex, const QList<PDFRenderError>& errors);

private:
    void onPageCompiled();

    struct CompileTask
    {
        QFuture<PDFPrecompiledPage> taskFuture;
        QFutureWatcher<PDFPrecompiledPage>* taskWatcher = nullptr;
    };

    PDFDrawWidgetProxy* m_proxy;
    State m_state = State::Inactive;
    QCache<PDFInteger, PDFPrecompiledPage> m_cache;
    std::map<PDFInteger, CompileTask> m_tasks;
};

}   // namespace pdf

#endif // PDFCOMPILER_H