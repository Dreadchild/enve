// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "imagesequencecachehandler.h"

#include "GUI/edialogs.h"

#include "filesourcescache.h"
#include "fileshandler.h"

void ImageSequenceFileHandler::afterPathSet(const QString &folderPath) {
    Q_UNUSED(folderPath)
    reload();
}

ImageCacheContainer* ImageSequenceFileHandler::getFrameAtFrame(const int relFrame) {
    if(mFrameImageHandlers.isEmpty()) return nullptr;
    const auto& cacheHandler = mFrameImageHandlers.at(relFrame);
    if(!cacheHandler) return nullptr;
    return cacheHandler->getImageContainer();
}

ImageCacheContainer *ImageSequenceFileHandler::getFrameAtOrBeforeFrame(
        const int relFrame) {
    if(mFrameImageHandlers.isEmpty()) return nullptr;
    if(relFrame >= mFrameImageHandlers.count()) {
        return mFrameImageHandlers.last()->getImageContainer();
    }
    const auto& cacheHandler = mFrameImageHandlers.at(relFrame);
    return cacheHandler->getImageContainer();
}

eTask *ImageSequenceFileHandler::scheduleFrameLoad(const int frame) {
    if(mFrameImageHandlers.isEmpty()) return nullptr;
    const auto& imageHandler = mFrameImageHandlers.at(frame);
    if(imageHandler->hasImage()) return nullptr;
    return imageHandler->scheduleLoad();
}

void ImageSequenceFileHandler::reload() {
    mFrameImageHandlers.clear();
    QDir dir(mPath);
    mFileMissing = !dir.exists();
    if(mFileMissing) return;
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    const auto files = dir.entryInfoList();
    for(const auto& fileInfo : files) {
        const auto suffix = fileInfo.suffix();
        if(!isImageExt(suffix)) continue;
        const auto filePath = fileInfo.absoluteFilePath();
        using IFDH = ImageFileDataHandler;
        const auto handler = IFDH::sGetCreateDataHandler<IFDH>(filePath);
        handler->clearCache();
        mFrameImageHandlers << handler;
    }
    if(mFrameImageHandlers.isEmpty()) mFileMissing = true;
}

void ImageSequenceFileHandler::replace() {
    const auto dir = eDialogs::openDir("Import Image Sequence", mPath);
    if(!dir.isEmpty()) setPath(dir);
}

ImageSequenceCacheHandler::ImageSequenceCacheHandler(
        ImageSequenceFileHandler *fileHandler) :
    mFileHandler(fileHandler) {}
