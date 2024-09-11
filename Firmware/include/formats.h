struct FilmFormat
{
  int id;
  String name;
  int sensor[22];
  int frame[22];
  int frame_fill[4];
};

FilmFormat film_formats[] = {
  {1, "Instax", {}, {}, {100,100}}
};

