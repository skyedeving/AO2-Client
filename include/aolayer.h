#ifndef AOLAYER_H
#define AOLAYER_H

#include <QDebug>
#include <QElapsedTimer>
#include <QImageReader>
#include <QLabel>
#include <QTimer>

class AOApplication;

class AOLayer : public QLabel {
  Q_OBJECT

public:
  AOLayer(QWidget *p_parent, AOApplication *p_ao_app);

  QString filename;    // file name without extension, i.e. "witnesstestimony"
  int static_duration; // time in ms for static images to be displayed, if
                       // applicable. set to 0 for infinite
  int max_duration;    // maximum duration in ms, image will be culled if it is
                       // exceeded. set this to 0 for infinite duration
  bool play_once = false;  // Whether to loop this animation or not
  bool cull_image = false; // if we're done playing this animation, should we
                           // hide it? overridden by above durations

  // used for effect loading because otherwise I'd have to rewrite most of the
  // effect code to support literally every subclass of AOLayer
  QString m_char = "";
  QString m_emote = "";

  // Set the movie's image to provided paths, preparing for playback.
  void start_playback(QString p_image);

  void set_play_once(bool p_play_once);

  // Stop the movie, clearing the image
  void stop();

  // Set the m_flipped variable to true/false
  void set_flipped(bool p_flipped) { m_flipped = p_flipped; }

  // Set the movie's playback speed (between 10% and 1000%)
  void set_speed(int modifier) { speed = qMax(10, qMin(modifier, 1000)); }

  // Move the label itself around
  void move(int ax, int ay);

  // This is somewhat pointless now as there's no "QMovie" object to resize, aka
  // no "combo" to speak of
  void combo_resize(int w, int h);

  // Return the frame delay adjusted for speed
  int get_frame_delay(int delay);

  // networked frame fx nonsense
  QStringList network_strings;

protected:
  AOApplication *ao_app;
  QVector<QPixmap> movie_frames;
  QVector<int> movie_delays;

  // Effects such as sfx, screenshakes and realization flashes are stored in
  // here. QString entry format: "sfx^[sfx_name]", "shake", "flash". The program
  // uses the QVector index as reference.
  QVector<QVector<QString>> movie_effects;

  QTimer *preanim_timer;
  QTimer *shfx_timer;
  QTimer *ticker;
  QString last_path;
  QImageReader m_reader;

  QElapsedTimer actual_time;

  // Usually used to turn seconds into milliseconds such as for [Time] tag in
  // char.ini
  const int tick_ms = 60;

  // These are the X and Y values before they are fixed based on the sprite's
  // width.
  int x = 0;
  int y = 0;
  // These are the width and height values before they are fixed based on the
  // sprite's width.
  int f_w = 0;
  int f_h = 0;

  int frame = 0;
  int max_frames = 0;
  int last_max_frames = 0;

  int speed = 100;

  bool m_flipped = false;
  // Are we loading this from the same frame we left off on? TODO: actually make
  // this work
  bool continuous = false;
  // Whether or not to forcibly bypass the simple check done by start_playback
  // and use the existent value of continuous instead
  bool force_continuous = false;

  int duration = 0;

  // iterate through a list of paths and return the first entry that exists. if
  // none exist, return NULL (safe because we check again for existence later)
  QString find_image(QList<QString> p_list);

  // Start playback of the movie (if animated).
  void play();

  // Freeze the movie at the current frame.
  void freeze();

  // Retreive a pixmap adjused for mirroring/aspect ratio shenanigans from a
  // provided QImage
  QPixmap get_pixmap(QImage image);

  // Set the movie's frame to provided pixmap
  void set_frame(QPixmap f_pixmap);

  // Initialize the frame-specific effects from the char.ini
  void load_effects();

  // Initialize the frame-specific effects from the provided network_strings,
  // this is only initialized if network_strings has size more than 0.
  void load_network_effects();

  // Play a frame-specific effect, if there's any defined for that specific
  // frame.
  void play_frame_effect(int frame);

signals:
  void done();
  void shake();
  void flash();
  void play_sfx(QString sfx);

private slots:
  void preanim_done();
  void shfx_timer_done();
  void movie_ticker();
};

class BackgroundLayer : public AOLayer {
  Q_OBJECT
public:
  BackgroundLayer(QWidget *p_parent, AOApplication *p_ao_app);
  void load_image(QString p_filename);
};

class ForegroundLayer : public AOLayer {
  Q_OBJECT
public:
  ForegroundLayer(QWidget *p_parent, AOApplication *p_ao_app);
  QString miscname; //'misc' folder to search. we get this from the caller
  void load_image(QString p_filename, QString p_miscname);
};

class CharLayer : public AOLayer {
  Q_OBJECT
public:
  CharLayer(QWidget *p_parent, AOApplication *p_ao_app);
  QString charname = "";      // name of the character folder to search
  QString current_emote = ""; // name of the emote we're using
  bool is_preanim;     // equivalent to the old play_once, if true we don't want
                       // to loop this
  QString prefix = ""; // prefix, left blank if it's a preanim

  void load_image(QString p_filename, QString p_charname, int p_duration);
  void play();

private:
  QString last_char;        // name of the last character we used
  QString last_emote;       // name of the last animation we used
  QString last_prefix;      // prefix of the last animation we played
  bool was_preanim = false; // whether is_preanim was true last time
};

class InterjectionLayer : public AOLayer {
  Q_OBJECT
public:
  InterjectionLayer(QWidget *p_parent, AOApplication *p_ao_app);
  void load_image(QString p_filename, QString p_charname, QString p_miscname);
};

class EffectLayer : public AOLayer {
  Q_OBJECT
public:
  EffectLayer(QWidget *p_parent, AOApplication *p_ao_app);
  void load_image(QString p_filename, bool p_looping);
};

class InterfaceLayer : public AOLayer {
  Q_OBJECT
public:
  InterfaceLayer(QWidget *p_parent, AOApplication *p_ao_app);
  void load_image(QString p_filename, QString p_miscname);
};
#endif // AOLAYER_H
