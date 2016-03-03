#include "musicsongtag.h"
#include "musictime.h"
#include "musicobject.h"

#include <QStringList>
#include <QPluginLoader>
#include <QFileInfo>
///qmmp incldue
#include "decoderfactory.h"
#include "metadatamodel.h"
#include "decoder.h"
///

MusicSongTag::MusicSongTag()
{
    m_tag = nullptr;
}

MusicSongTag::~MusicSongTag()
{
    delete m_tag;
}

bool MusicSongTag::readFile(const QString &file)
{
    delete m_tag;
    m_tag = new TagReadAndWrite(file);
    if(!m_tag->readFile())
    {
        return readOtherTaglibNotSupport(file);
    }

    m_parameters = m_tag->getMusicTags();
    return true;
}

bool MusicSongTag::readOtherTaglibNotSupport(const QString &path)
{
    QPluginLoader loader;
    QString suffix = QFileInfo(path).suffix().toLower();

    if(suffix == "aac")
    {
#ifdef Q_OS_WIN
#  ifdef MUSIC_QT_5
        loader.setFileName("plugins/Input/aac.dll");
#  else
        loader.setFileName("../bin/plugins/Input/aac.dll");
#  endif
#elif defined Q_OS_UNIX
#  ifdef MUSIC_QT_5
        loader.setFileName("qmmp/Input/aac.dll");
#  else
        loader.setFileName("../lib/qmmp/Input/aac.dll");
#  endif
#endif
    }
    else if(suffix == "mid")
    {
#ifdef Q_OS_WIN
#  ifdef MUSIC_QT_5
        loader.setFileName("plugins/Input/wildmidi.dll");
#  else
        loader.setFileName("../bin/plugins/Input/wildmidi.dll");
#  endif
#elif defined Q_OS_UNIX
#  ifdef MUSIC_QT_5
        loader.setFileName("qmmp/Input/wildmidi.dll");
#  else
        loader.setFileName("../lib/qmmp/Input/wildmidi.dll");
#  endif
#endif
    }

    QObject *obj = loader.instance();
    DecoderFactory *decoderfac = NULL;
    if(obj && (decoderfac = qobject_cast<DecoderFactory*>( obj )) )
    {
        MetaDataModel *model = decoderfac->createMetaDataModel(path);
        if(model != NULL)
        {
            QHash<QString, QString> datas = model->audioProperties();
            MusicTime t = MusicTime::fromString(datas.value("Length"), QString("m:ss"));
            QString ts = QString::number(t.getTimeStamp(MusicTime::All_Msec));
            m_parameters.insert(TagReadAndWrite::TAG_LENGTH, ts);
            m_parameters.insert(TagReadAndWrite::TAG_SAMPLERATE, datas.value("Sample rate"));
            m_parameters.insert(TagReadAndWrite::TAG_BITRATE, datas.value("Bitrate"));
        }

        QList<FileInfo*> infos(decoderfac->createPlayList(path, true, 0));
        if(!infos.isEmpty())
        {
            QString t = QString::number(infos.first()->length()*1000);
            m_parameters.insert(TagReadAndWrite::TAG_LENGTH, t);
        }
    }

    return !m_parameters.isEmpty();
}

QString MusicSongTag::getArtist() const
{
    return m_parameters[TagReadAndWrite::TAG_ARTIST];
}

QString MusicSongTag::getTitle() const
{
    return m_parameters[TagReadAndWrite::TAG_TITLE];
}

QString MusicSongTag::getAlbum() const
{
    return m_parameters[TagReadAndWrite::TAG_ALBUM];
}

QString MusicSongTag::getComment() const
{
    return m_parameters[TagReadAndWrite::TAG_COMMENT];
}

QString MusicSongTag::getYear() const
{
    return m_parameters[TagReadAndWrite::TAG_YEAR];
}

QString MusicSongTag::getTrackNum() const
{
    return m_parameters[TagReadAndWrite::TAG_TRACK];
}

QString MusicSongTag::getGenre() const
{
    return m_parameters[TagReadAndWrite::TAG_GENRE];
}

QString MusicSongTag::getAlbumArtist() const
{
    return m_parameters[TagReadAndWrite::TAG_ALBUMARTIST];
}

QString MusicSongTag::getComposer() const
{
    return m_parameters[TagReadAndWrite::TAG_COMPOSER];
}

QString MusicSongTag::getChannel() const
{
    return m_parameters[TagReadAndWrite::TAG_CHANNEL];
}

QString MusicSongTag::getURL() const
{
    return m_parameters[TagReadAndWrite::TAG_URL];
}

/////////////////////////////////////////////
void MusicSongTag::setArtist(const QString &artist)
{
    m_tag->writeMusicTag(TagReadAndWrite::TAG_ARTIST, artist);
}

void MusicSongTag::setTitle(const QString &title)
{
    m_tag->writeMusicTag(TagReadAndWrite::TAG_TITLE, title);
}

void MusicSongTag::setAlbum(const QString &album)
{
    m_tag->writeMusicTag(TagReadAndWrite::TAG_ALBUM, album);
}

void MusicSongTag::setComment(const QString &comment)
{
    m_tag->writeMusicTag(TagReadAndWrite::TAG_COMMENT, comment);
}

void MusicSongTag::setYear(const QString &year)
{
    m_tag->writeMusicTag(TagReadAndWrite::TAG_YEAR, year);
}

void MusicSongTag::setTrackNum(const QString &track)
{
    m_tag->writeMusicTag(TagReadAndWrite::TAG_TRACK, track);
}

void MusicSongTag::setGenre(const QString &genre)
{
    m_tag->writeMusicTag(TagReadAndWrite::TAG_GENRE, genre);
}

/////////////////////////////////////////////
QString MusicSongTag::getSamplingRate() const
{
    return m_parameters[TagReadAndWrite::TAG_SAMPLERATE];
}

QString MusicSongTag::getFormat() const
{
    return m_parameters[TagReadAndWrite::TAG_FORMAT];
}

QString MusicSongTag::getMode() const
{
    return m_parameters[TagReadAndWrite::TAG_MODE];
}

QString MusicSongTag::getBitrate() const
{
    return m_parameters[TagReadAndWrite::TAG_BITRATE];
}

QString MusicSongTag::getLengthString() const
{
    return MusicTime::msecTime2LabelJustified(
           m_parameters[TagReadAndWrite::TAG_LENGTH].toULongLong());
}