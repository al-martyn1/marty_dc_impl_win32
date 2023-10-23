// php coloring <?php

class lang
{
  static name_belka = "Чиви";
  static name_robot = "Робот лесоруб";
  static title_actions = "действия:";
}

// helpers

class aux
{
  static min = function(a, b)
  {
    return (a > b ? b : a);
  }
  
  static max = function(a, b)
  {
    return (a < b ? b : a);
  }
  
  static min_v = function(first, ...)
  {
    switch( argv.len() )
    {
      case 0: { return first; }
      case 1: { return min(first, argv[0]); }
    }
    argv[0] = min(first, argv[0]);
    return aux.min_v.call(this, argv);
  }
  
  static max_v = function(first, ...)
  {
    switch( argv.len() )
    {
      case 0: { return first; }
      case 1: { return max(first, argv[0]); }
    }
    argv[0] = max(first, argv[0]);
    return aux.max_v.call(this, argv);
  }
  
  static implode = function(array, sep)
  {
    local len = array.len();
    switch( len )
    {
      case 0: {
        return "";
      }
      
      case 1: {
        return array[0].tostring();
      }
    }
    
    local ret = array[0].tostring();
    for( local i = 1; i < len; i += 1 )
    {
      ret += sep + array[i].tostring();
    }
    
    return ret;
  }
}

class range
{
  from  = 0;
  to    = 0;
  
  constructor(n_from, n_to)
  {
    from  = n_from;
    to    = n_to;
  }
  
  function clamp(n)
  {
    return aux.max( from, aux.min(n, to) );
  }
  
  function fit(n)
  {
    return (from <= n && n <= to);
  }
}

class limited
{
  range   = null;
  value   = 0;
  
  constructor(n_value, p_range)
  {
    range = p_range;
    value = range.clamp(n_value);
  }
  
  function next()
  {
    value = aux.min(value + 1, range.to);
  }
  
  function prev()
  {
    value = aux.max(value - 1, range.from);
  }
  
  function next_wrap()
  {
    value = (value >= range.to ? range.from :
      aux.min(value + 1, range.to)
    );
  }
  
  function prev_wrap()
  {
    value = (value <= range.from ? range.to :
      aux.max(value - 1, range.from)
    );
  }
  
  function change(n_value) // returns true if changed
  {
    return (range.fit(n_value) ? ( (value = n_value), true ) : false);
  }
}

class selection
{
  options = null;
  index   = null;
  
  constructor(a_options)
  {
    options = a_options;
    index   = limited( 0, range(0, options.len() - 1) );
  }
  
  function current()
  {
    return options[index.value];
  }
}

class blinker
{
  counter = 0;
  frames_on = 17;
  frames_end = 23;
  
  constructor(i_frames_on, i_frames_off)
  {
    frames_on = i_frames_on;
    frames_end = i_frames_on + i_frames_off;
  }
  
  function blink()
  {
    local ret = status();
    next();
    return ret;
  }
  
  function next()
  {
    if( counter >= frames_end )
    {
      counter = 0;
    } else {
      counter += 1;
    }
  }
  
  function status()
  {
    return (counter < frames_on);
  }
  
  function reset()
  {
    counter = 0;
  }
}

//

D <- Drawing;

function format_coord(c)
{
  return "[" + c.x.tostring() + ", " + c.y.tostring() + "]";
}

function text_out(dc, array_text, pos, step, font, color)
{
  dc.setTextColor(color);
  dc.selectFont(font);
  foreach(text in array_text)
  {
    dc.textOut(pos, text);
    pos += step;
  }
}

function text_out_ex(dc, array_text, pos, step, font)
{
  dc.selectFont(font);
  foreach(text in array_text)
  {
    dc.setTextColor(text[1]);
    dc.textOut(pos, text[0]);
    pos += step;
  }
}

class coords
{
  x = 0;
  y = 0;
  
  constructor(i_x, i_y) { x = i_x.tointeger(); y = i_y.tointeger(); }
  
  function get() { return D.Coords(x, y); }
  
  function _add(other) { return ::coords(x + other.x, y + other.y); }
  
  function _sub(other) { return ::coords(x - other.x, y - other.y); }
  
  function _unm() { return ::coords(-x, -y); }
  
  function _mul(other) { return ::coords(x * other.x, y * other.y); }
  
  function _div(other) { return ::coords(x / other.x, y / other.y); }
}

class area
{
  pos = null;
  size = null;
  
  constructor(x, y, w, h)
  {
    pos = coords(x, y);
    size = coords(w, h);
  }
  
  function copy() { return from_coords(pos, size); }
  
  function move_centered_to(pos_center)
  {
    pos = pos_center - size / coords(2, 2);
  }
  
  function inner_to() { return pos + size; }
  function inner_centered() { return pos + size / coords(2, 2); }
  function inner_bottom() { return pos + coords(0, size.y); }
  function inner_right() { return pos + coords(size.x, 0); }
  
  function from() { return pos.get(); }
  
  function to(scr) { return inner_to().get() - scr.one; }
  
  function bottom(scr)
  {
    local ret = inner_bottom().get();
    ret.y -= scr.one.y;
    return ret;
  }
  
  function right(scr)
  {
    local ret = inner_right().get();
    ret.x -= scr.one.x;
    return ret;
  }
  
  static from_coords = function(p_pos, p_size)
  { return ::area(p_pos.x, p_pos.y, p_size.x, p_size.y); }
}

function area::draw_triangle_up(dc, scr, shrink)
{
  local prec = dc.setDrawingPrecise( D.DrawingPrecise.SmoothingPrecise );
  
  dc.selectPen(scr.sel_pen);
  dc.selectBrush(scr.sel_brush);
  
  dc.beginPath();
  
  dc.moveTo( to(scr) - D.Coords(shrink, 0) );
  dc.lineTo( bottom(scr) + D.Coords(shrink, 0) );
  dc.lineTo( from() + D.Coords(size.x.tofloat() / 2, 0) );
  dc.closeFigure();
  
  dc.endPath(true, true);
  
  dc.setDrawingPrecise( prec );
}

function area::draw_triangle_up_truncated(dc, scr, shrink, side)
{
  local prec = dc.setDrawingPrecise( D.DrawingPrecise.SmoothingPrecise );
  
  dc.selectPen(scr.sel_pen);
  dc.selectBrush(scr.sel_brush);
  
  dc.beginPath();
  
  local pos_start = to(scr) - D.Coords(shrink, 0);
  dc.moveTo( pos_start );
  local pos = bottom(scr) + D.Coords(shrink, 0);
  dc.lineTo( pos );
  pos -= D.Coords(0, side);
  dc.lineTo( pos );
  dc.lineTo( from() + D.Coords(size.x.tofloat() / 2, 0) );
  pos_start -=  D.Coords(0, side);
  dc.lineTo( pos_start );
  dc.closeFigure();
  
  dc.endPath(true, true);
  
  dc.setDrawingPrecise( prec );
}

// ui config

class resolution
{
  static w = 800;
  static h = 600;
}

class config
{
  static zero = coords(0, 0);
  static size = coords(resolution.w, resolution.h);
  
  static f_zero         = D.Coords(0, 0);
  static f_size         = D.Coords(resolution.w, resolution.h);
  static f_aspect_ratio = D.Coords(4, 3);
}

class grid_skills
{
  padding       = coords(10, 10);
  padding_under = 8;
  title_pos     = null;
  
  icon_size     = coords(40, 40);
  icon_spacing  = 20;
  icon_step     = 0;
  
  sel_blink     = blinker(22, 7);
  
  constructor(about_area)
  {
    title_pos = about_area.pos + padding;
    
    icon_step = icon_size.x + icon_spacing;
  }
}

function grid_skills::update(tick_delta)
{
  sel_blink.next();
}

function grid_skills::draw(dc, scr, skills)
{
  // icons
  local at = area.from_coords( title_pos, coords(40, 40) );
  foreach( skill in skills.options )
  {
    skill.info.icon.draw(dc, scr, at);
    at.pos.x += icon_size.x + icon_spacing;
  }
  
  local hotkey_x = at.inner_right().x;
  dc.textOutWithFontAndColor(
    at.right(scr),
    scr.next_font,
    ui.next_color,
    "< Left | Right >"
  );
  
  local top = at.inner_to().y + padding_under;
  local offset_x = skills.index.value * icon_step;
  
  // selection arrow
  if( sel_blink.status() )
  {
    at.pos = coords(padding.x + offset_x, top);
    at.size.y = padding.y;
    
    at.draw_triangle_up(dc, scr, 6);
  }
  
  local cur_skill = skills.current();
  
  // current skill name
  top += padding_under * 2;
  dc.textOutWithFontAndColor(D.Coords(padding.x /*+ offset_x*/, top), scr.about_font, ui.about_title_color, cur_skill.info.name);
  
  dc.textOutWithFontAndColor(
    D.Coords(hotkey_x /*+ offset_x*/, config.size.y - 20),
    scr.next_font,
    ui.next_color,
    "[ Space / Enter ]"
  );
  
  top += 20;
  
  // current skill notes
  text_out(dc, cur_skill.info.note, D.Coords(padding.x /*+ offset_x*/, top), D.Coords(0, 20), scr.skill_note_font, ui.about_color);
  
  // todo: apply button | help F1
  //dc.textOutWithFontAndColor(title_pos.get(), scr.about_font, ui.about_title_color, lang.title_actions);
}

class grid_all
{
  hp_belka  = area(100, 10, 200, 30);
  hp_enemy  = area(500, 10, 200, 30);
  work      = area(0, 50, resolution.w, 400);
  about     = null;
  
  skills  = null;
  
  constructor()
  {
    local about_y = work.inner_to().y;
    about = area(0, about_y, resolution.w, resolution.h - about_y);
    
    skills = grid_skills(about);
  }
}

class ui
{
  // colors
  
  static bg_color           = D.Color.fromRgb(200, 200, 200);
  static hp_color_border    = D.Color(0xaa0011);
  static title_color        = D.Color(0x7a7afa);
  static about_title_color  = D.Color(0x2020ff);
  static about_color        = D.Color(0x323232);
  static about_color_belka  = D.Color(0xff6300);
  static about_color_other  = D.Color(0x0063fa);
  static next_color         = D.Color(0x801050); //D.Color(0x952eff);
  
  static img_color_border   = D.Color.fromRgb(0, 0, 200);
  static img_color_text     = D.Color.fromRgb(200, 63, 63);
  
  // coords
  
  static grid       = grid_all();
  static title_pos  = D.Coords(17, 11);
  
  // pens
  
  static def_pen_params   = D.PenParams(1, D.LineEndcapStyle.Flat, D.LineJoinStyle.Mitter);
  static sel_pen_params   = D.PenParams(2, D.LineEndcapStyle.Flat, D.LineJoinStyle.Round);
  
  // gradients
  
  static gradient_hp_bar_belka = D.GradientParams(
    D.Color(0xbb2211), D.Color(0xe5e500), D.Color(0x88dd55), 0.25
  );
  
  static gradient_hp_bar_robot = D.GradientParams(
    D.Color(0x13A701), D.Color(0x517BDB), D.Color(0xfafafa), 0.5
  );
  
  // font params
  
  static img_font_params  = D.FontParams(
    11,
    D.FontWeight.Normal,
    D.FontStyleFlags.Normal,
    "Verdana"
  );
  
  static title_font_params  = D.FontParams(
    22,
    D.FontWeight.Normal,
    D.FontStyleFlags.Normal,
    "Arial"
  );
  
  static about_font_params  = D.FontParams(
    18,
    D.FontWeight.Normal,
    D.FontStyleFlags.Normal,
    "Calibri"
  );
  
  static next_font_params  = D.FontParams(
    14,
    D.FontWeight.Normal,
    D.FontStyleFlags.Normal,
    "Courier New"
  );
  
  static skill_note_font_params  = D.FontParams(
    20,
    D.FontWeight.Normal,
    D.FontStyleFlags.Normal,
    "Garamond"
  );
}

class screen
{
  factor = 1;
  one = null;
  
  def_pen         = null;
  img_pen_border  = null;
  hp_pen_border   = null;
  sel_pen         = null;
  
  bg_brush        = null;
  sel_brush       = null;
  
  img_font        = null;
  
  title_font      = null;
  about_font      = null;
  next_font       = null;
  skill_note_font = null;
  
  constructor(dc)
  {
    def_pen         = dc.getCurPen();
    img_pen_border  = dc.createSolidPen(ui.def_pen_params, ui.img_color_border);
    hp_pen_border   = dc.createSolidPen(ui.def_pen_params, ui.hp_color_border);
    sel_pen         = dc.createSolidPen(ui.sel_pen_params, ui.about_color_belka);
    
    bg_brush        = dc.createSolidBrush(ui.bg_color);
    sel_brush       = dc.createSolidBrush( D.Color.fromRgb(230, 230, 0) );
    
    init_scale(dc);
    
    img_font        = dc.createFont(ui.img_font_params);
    
    title_font      = dc.createFont(ui.title_font_params);
    about_font      = dc.createFont(ui.about_font_params);
    next_font       = dc.createFont(ui.next_font_params);
    skill_note_font = dc.createFont(ui.skill_note_font_params);
  }
  
  function init_scale(dc)
  {
    dc.setScale( D.Scale(1, 1) );
    
    local size = dc.getSize();
    local k_sz = size / config.f_aspect_ratio;
    
    local offset;
    if( k_sz.x < k_sz.y )
    {
      factor = size.x / config.f_size.x;
      dc.setScale( D.Scale(factor, factor) );
      size = dc.getSize();
      offset = D.Coords( 0, (size.y - config.f_size.y) / 2 );
    } else {
      factor = size.y / config.f_size.y;
      dc.setScale( D.Coords(factor, factor) );
      size = dc.getSize();
      offset = D.Coords( (size.x - config.f_size.x) / 2, 0 );
    }
    size = dc.getSize();
    dc.setOffset(offset);
    
    one = dc.getPixelSize(); //dc.mapRawToLogicSize( D.Coords(1, 1) );
    
    dc.selectBrush(bg_brush);
    dc.fillRect( D.Coords(0, 0), D.Coords(resolution.w, resolution.h) - one, false);
  }
  
  function debug_info()
  {
    dc.textOut(
      D.Coords(3, 3),
      "scale: " + format_coord( D.Coords(factor, factor) ) +
      " offset: " + format_coord(offset) +
      " full size: " + format_coord( dc.getSize() )
    );
  }
}

// shapes

// форма фигуры - способ рисования
class shape_none
{
  function draw(dc, scr, at) {}
  function change(param) {}
}

class shape_image extends shape_none
{
  id = "none";
  
  constructor(p_id) { id = p_id; }
  
  function draw(dc, scr, at)
  {
    dc.selectPen(scr.img_pen_border);
    dc.rect( at.from(), at.to(scr) );
    
    local tip = "img: ";
    if( at.size.x > 45 ) { tip += id; }
    
    dc.textOutWithFontAndColor(
      (at.pos + coords(3, 3)).get(),
      scr.img_font,
      ui.img_color_text,
      tip
    );
  }
}

class shape_hp_bar_gradient extends shape_none
{
  gradient = null;
  ratio = 1.0;
  
  static gradient_fill_flags
  = D.GradientRoundRectFillFlags.NoFillEnd
  | D.GradientRoundRectFillFlags.SquareBegin
  | D.GradientRoundRectFillFlags.SquareEnd
  ;
  
  constructor(p_gradient)
  {
    gradient = p_gradient;
  }
  
  function draw(dc, scr, at)
  {
    if( ratio > 0.0 )
    {
      // 2023-10-13: fillGradientRoundRect() работает неправильно и при true и при false
      // todo: ask Marty to fix (when first level done)
      dc.fillGradientRoundRect(0,
        //at.from(), at.to(scr), gradient, D.GradientType.Vertical, true,
        at.from(), at.to(scr), gradient, D.GradientType.Horizontal, true,
        ratio, gradient_fill_flags
      );
    }
    
    dc.selectPen(scr.hp_pen_border);
    dc.rect( at.from(), at.to(scr) );
  }
  
  function change(param) { ratio = param; }
}

// у фигуры позиция и форма
class figure
{
  shape = null;
  at    = null;
  
  constructor(shape_p, area_at)
  {
    shape = shape_p;
    at    = area_at;
  }
  
  function draw(dc, scr) { shape.draw(dc, scr, at); }
}

none_figure <- figure( shape_none, area(0, 0, 1, 1) );

// animations

class mover_linear
{
  duration = 1000;
  
  function calc_pos(tick_delta, pos_current, pos_target)
  {
    if( tick_delta > duration ) { tick_delta = duration; }
    if( duration < 1 ) { return pos_target; }
    
    local pos_offset = (pos_target - pos_current);
    pos_offset = pos_offset * coords(tick_delta, tick_delta) / coords(duration, duration);
    
    duration -= tick_delta;
    
    return pos_current + pos_offset;
  }
  
  function is_done() { return (duration < 1); }
  
  function reset(time_duration)
  {
    duration = time_duration;
  }
}

//

class projectile
{
  mover = null;
  sprite = null;
  
  constructor(p_mover, p_sprite)
  {
    mover = p_mover;
    sprite = p_sprite;
  }
  
  function move(tick_delta, pos_target)
  {
    local pos_center = mover.calc_pos(tick_delta, sprite.at.inner_centered(), pos_target);
    sprite.at.move_centered_to(pos_center);
  }
  
  function reset(time_duration, pos_src)
  {
    mover.reset(time_duration);
    sprite.at.move_centered_to(pos_src);
  }
}

// game api

class hp_info
{
  max       = 100;
  current   = 100;
  
  indicator = none_figure;
  
  constructor(i_max = 100, i_current = null)
  {
    max     = i_max;
    current = (i_current == null ? i_max : i_current);
  }
  
  function ratio() { return current.tofloat() / max; }
  
  function hurt(amount)
  {
    if( current > amount ) { current -= amount; } else { current = 0; }
    
    indicator.shape.change( this.ratio() );
  }
  
  function heal(amount)
  {
    local v = current + amount;
    current = ( v < max ? v : max);
    
    indicator.shape.change( this.ratio() );
  }
}

// power-up system

class expiration_none
{
  function on_turn() { }
  function is_done() { return false; }
}

class expiration_timed extends expiration_none
{
  counter = 1;
  
  constructor(i_counter = 1) { counter = i_counter; }
  
  function on_turn() { if( counter > 0 ) { counter -= 1; } }
  function is_done() { return (counter < 1); }
}

class buff_info
{
  value       = 0;
  expiration  = expiration_none();
  
  constructor(i_value = 0, i_turns_to_end = null)
  {
    value = i_value;
    if( i_turns_to_end != null )
    {
      expiration = expiration_timed(i_turns_to_end);
    }
  }
  
  function on_turn()
  {
    expiration.on_turn();
    if( expiration.is_done() )
    {
      value = 0;
      expiration = expiration_none();
    }
  }
}

// game entity

class unit
{
  name    = "";
  hp      = null;
  evasion = buff_info(); // .value is chance %
  shield  = buff_info(); // .value is damage mitigation %
  
  sprite  = none_figure;
  
  offset_src = coords(0, 30);
  offset_dst = coords(30, 0);
  
  function src() { return sprite.at.pos + offset_src; }
  function dst() { return sprite.at.pos + offset_dst; }
  
  constructor(str_name, i_hp)
  {
    name = str_name;
    hp = hp_info(i_hp);
  }
  
  function draw(dc, scr)
  {
    hp.indicator  .draw(dc, scr);
    sprite        .draw(dc, scr);
  }
  
  function add_hp_indicator(p_gradient, at)
  {
    hp.indicator = figure( shape_hp_bar_gradient(p_gradient), at );
  }
}

// abilities

class skill_info
{
  icon = null;
  name = "";
  note = [""];
  
  constructor(idx_icon, str_name, arr_note)
  {
    icon = shape_image(idx_icon);
    name = str_name;
    note = arr_note;
  }
}

class skill_usage
{
  source = null;
  target = null;
  
  constructor(uint_source, unit_target)
  {
    source = uint_source;
    target = unit_target;
  }
}

class skill_action_none
{
  function apply(scene, usage) { return false; }
}

class skill
{
  info    = null;
  action  = null; // action.apply(..) ~ to starts animation
  effect  = null; // effect.apply(..) ~ after animation ends
  
  constructor(p_info, p_action, p_effect)
  {
    info    = p_info;
    action  = p_action;
    effect  = p_effect;
  }
}

// skill effects

class skill_effect_hurt extends skill_action_none
{
  amount          = 0;
  evadable        = true;
  ignores_shield  = false;
  
  constructor(i_amount, b_evadable, b_ignores_shield)
  {
    amount          = i_amount;
    evadable        = b_evadable;
    ignores_shield  = b_ignores_shield;
  }
  
  function apply(scene, usage)
  {
    local target = usage.target;
    
    if( evadable && target.evasion.value == 100 ) { return false; }
    
    do_hurt( target, calc_for(target) );
    
    return true;
  }
  
  function calc_for(target)
  {
    return (
      (target.shield.value > 0 && ignores_shield == false) ?
      (amount * target.shield.value / 100) :
      amount
    );
  }
  
  function do_hurt(target, value)
  {
    target.hp.hurt(value);
  }
}

// skill collections

class skills_belka
{
  static throw_acorn = function()
  {
    return skill(
      skill_info(
        "skills.belka/icon.acorn",
        "Метнуть орех",
        ["наносит весомый урон по цели"]
      ),
      skill_action_none(),
      skill_effect_hurt(40, false, false)
    );
  };
  
  static jump_strike = function()
  {
    return skill(
      skill_info(
        "skills.belka/icon.strike",
        "Удар с прыжка",
        ["меньший урон", "зато игнорирует щит"]
      ),
      skill_action_none(),
      skill_effect_hurt(40, false, false)
    );
  };
  
  static try_evade = function()
  {
    return skill(
      skill_info(
        "skills.belka/icon.dodge",
        "Попытка уклонения",
        ["летящие снаряды по Чиви промажут", "время действия: 1 ход"]
      ),
      skill_action_none(),
      skill_action_none()
    );
  };
}

// basic scene information

class scene_none
{
  static number_key_code = function(n_char)
  {
    switch( n_char )
    {
      case Vk.Code._1 : return 0;
      case Vk.Code._2 : return 1;
      case Vk.Code._3 : return 2;
      case Vk.Code._4 : return 3;
      case Vk.Code._5 : return 4;
      case Vk.Code._6 : return 5;
      case Vk.Code._7 : return 6;
      case Vk.Code._8 : return 7;
      case Vk.Code._9 : return 8;
      default : return null;
    }
  }
  
  function draw(dc, scr) {}
  function update(tick_delta) { return false; }
  function on_key(b_pushed, n_char, n_rep_cnt) { return false; }
  
  function on_next() {}
  function on_prev() {}
}

class game_info
{
  scene       = scene_none();
  next_scene  = null;
  
  function event_pre()
  {
    if( next_scene != null )
    {
      scene       = next_scene;
      next_scene  = null;
    }
  }
}

game_status <- game_info();

// intro base
class scene_intro extends scene_none
{
  title = "";
  note = [];
  
  next_blink = blinker(25, 8);
  
  static text_line_step = D.Coords(0, 20);
  
  function draw(dc, scr)
  {
    dc.textOutWithFontAndColor(ui.title_pos, scr.title_font, ui.title_color, title);
    local about_y = ui.grid.about.pos.y;
    local about_h = ui.grid.about.size.y;
    local note_h = (about_h - (note.len() + 0.5) * text_line_step.y);
    local note_pos = D.Coords(40, about_y + note_h / 2);
    text_out_ex(dc, note, note_pos, text_line_step, scr.about_font);
    
    if( next_blink.status() )
    {
      dc.textOutWithFontAndColor(
        (config.size - coords(145, 30)).get(),
        scr.next_font,
        ui.next_color,
        "Space / Enter >"
      );
    }
  }
  
  function draw_arrow_back(dc, scr)
  {
    dc.textOutWithFontAndColor(
      (ui.grid.about.inner_right() + coords(-110, ui.grid.skills.padding.y)).get(),
      scr.next_font,
      ui.next_color,
      "BackSpace △"
    );
  }
  
  function update(tick_delta)
  {
    next_blink.next();
    return true;
  }
  
  function on_key(b_pushed, n_char, n_rep_cnt)
  {
    if( b_pushed == false ) { return false; }
    
    switch( n_char )
    {
      // enter, space
      case Vk.Code.Enter :
      case Vk.Code.Space : {
        on_next();
        return true;
      }
      
      // backspace
      case Vk.Code.Back : {
        on_prev();
        return true;
      }
    }
    
    return false;
  }
}

// fight base
class scene_fight extends scene_none
{
  unit_belka = null;
  unit_enemy = null;
  
  is_player_turn = true;
  usages         = null;
  
  belka_skills = selection([]);
  
  constructor()
  {
    usages = [
      skill_usage(unit_enemy, unit_belka),
      skill_usage(unit_belka, unit_enemy)
    ];
  }
  
  function update(tick_delta)
  {
    ui.grid.skills.update(tick_delta);
    return true;
  }
  
  function on_key(b_pushed, n_char, n_rep_cnt)
  {
    if( b_pushed == false ) { return false; }
    
    // key: 1 .. 9
    local num = scene_none.number_key_code(n_char);
    if( num != null && belka_skills.index.change(num) )
    {
      ui.grid.skills.sel_blink.reset();
      return true;
    }
    
    switch( n_char )
    {
      // right arrow
      case Vk.Code.Right : {
        belka_skills.index.next();
        ui.grid.skills.sel_blink.reset();
        return true;
      }
      
      // left arrow
      case Vk.Code.Left : {
        belka_skills.index.prev();
        ui.grid.skills.sel_blink.reset();
        return true;
      }
      
      // backspace
      case Vk.Code.Back : {
        on_prev();
        return true;
      }
    }
    
    return false;
  }
  
  function draw(dc, scr)
  {
    ui.grid.skills.draw(dc, scr, belka_skills);
  }
  
  function get_usage() { return usages[is_player_turn.tointeger()]; }
}

// level 01: Robot Lumberjack

class scene_level_01_intro extends scene_intro
{
  bg    = null;
  belka = null;
  
  constructor()
  {
    title = "В беличьем дупле";
    note = [
      ["Белка Чиви проснулась от громких звуков с улицы:", ui.about_color],
      [" – Что это за шум? Пойду посмотрю...", ui.about_color]
    ];
    
    bg    = figure( shape_image("level.01/bg.intro"), ui.grid.work );
    belka = figure( shape_image("level.01/belka.intro"), area(50, 90, 200, 200) );
  }
  
  function draw(dc, scr)
  {
    base.draw(dc, scr);
    
    bg    .draw(dc, scr);
    belka .draw(dc, scr);
  }
  
  function on_next()
  {
    game_status.next_scene = scene_level_01_intro_outside();
  }
}

class scene_level_01_intro_outside extends scene_intro
{
  bg    = null;
  belka = null;
  robot = null;
  
  constructor()
  {
    title = "Защита жилища";
    note = [
      ["Чиви:", ui.about_color_belka],
      [" – Прекрати пилить! Тут мой дом!", ui.about_color],
      ["Робот лесоруб:", ui.about_color_other],
      [" – Обнаружена помеха. Приступаю к устранению.", ui.about_color],
      ["Чиви:", ui.about_color_belka],
      [" – Да я сама тебя сейчас устраню!", ui.about_color]
    ];
    
    bg    = figure( shape_image("level.01/bg"), ui.grid.work );
    belka = figure( shape_image("level.01/belka"), area(50, 90, 150, 150) );
    robot = figure( shape_image("level.01/robot"), area(450, 130, 300, 300) );
  }
  
  function draw(dc, scr)
  {
    base.draw(dc, scr);
    draw_arrow_back(dc, scr);
    
    bg    .draw(dc, scr);
    belka .draw(dc, scr);
    robot .draw(dc, scr);
  }
  
  function on_next()
  {
    game_status.next_scene = scene_level_01();
  }
  
  function on_prev()
  {
    game_status.next_scene = scene_level_01_intro();
  }
}

  // boss 1
class scene_level_01 extends scene_fight
{
  sprite_bg   = null;
  
  proj_nut    = null;
  proj_nut_is = false;
  
  constructor()
  {
    unit_belka = unit(lang.name_belka, 100);
    unit_enemy = unit(lang.name_robot, 200);
    
    unit_belka.add_hp_indicator(ui.gradient_hp_bar_belka, ui.grid.hp_belka);
    unit_enemy.add_hp_indicator(ui.gradient_hp_bar_robot, ui.grid.hp_enemy);
    
    unit_belka.offset_src = coords(130, 50);
    unit_enemy.offset_dst = coords(150, 90);
    
    unit_belka.sprite = figure( shape_image("level.01/belka"), area(50, 90, 150, 150) );
    unit_enemy.sprite = figure( shape_image("level.01/robot"), area(450, 130, 300, 300) );
    
    belka_skills = selection([
      skills_belka.throw_acorn(),
      skills_belka.jump_strike(),
      skills_belka.try_evade()
    ]);
    
    sprite_bg = figure( shape_image("level.01/bg"), ui.grid.work );
    
    proj_nut = projectile(
      mover_linear(),
      figure( shape_image("skills.belka/proj.acorn"), area(0, 0, 30, 30) )
    );
    
    base.constructor();
  }
  
  function on_key(b_pushed, n_char, n_rep_cnt)
  {
    if( b_pushed == false ) { return false; }
    
    switch( n_char )
    {
      case Vk.Code.Enter :
      case Vk.Code.Space : {
        if( belka_skills.index.value != 0 || proj_nut_is ) { return false; }
        proj_nut.reset( 500, unit_belka.src() );
        proj_nut_is = true;
        return true;
      }
    }
    
    return base.on_key(b_pushed, n_char, n_rep_cnt);
  }
  
  function update(tick_delta)
  {
    local ret = false;
    
    if( proj_nut_is )
    {
      ret = true;
      if( proj_nut.mover.is_done() ) { proj_nut_is = false; }
      else { proj_nut.move( tick_delta, unit_enemy.dst() ); }
    }
    
    return ret;
  }
  
  function draw(dc, scr)
  {
    sprite_bg     .draw(dc, scr);
    
    unit_belka    .draw(dc, scr);
    unit_enemy    .draw(dc, scr);
    
    base.draw(dc, scr);
    
    if( proj_nut_is )
    {
      proj_nut.sprite.draw(dc, scr);
    }
  }
  
  function on_prev()
  {
    game_status.next_scene = scene_level_01_intro_outside();
  }
}

// outer events

  // load
function Game::onLoad(b_first)
{
  game_status.scene = scene_level_01_intro();
  //game_status.scene = scene_level_01();
  return true;
}

  // update
function Game::onUpdate(tick_delta)
{
  game_status.event_pre();
  return game_status.scene.update(tick_delta);
}

  // keyboard
function Game::onKeyEvent(b_pushed, n_char, n_rep_cnt)
{
  game_status.event_pre();
  return game_status.scene.on_key(b_pushed, n_char, n_rep_cnt);
}

  // paint
function Game::onPaint(dc)
{
  game_status.event_pre();
  dc.setDrawingPrecise(D.DrawingPrecise.PixelPrecise);
  
  scr <- screen(dc);
  
  game_status.scene.draw(dc, scr);
  
  // debug marker
  dc.selectPen(scr.def_pen);
  dc.circle(D.Coords(config.f_size.x + 10, config.f_size.y - 10), 5);
  dc.circle(D.Coords(config.f_size.x - 10, config.f_size.y + 10), 5);
}

  // window size changing
function Game::onWindowSizing(flags_edge, size)
{
  return true;
}
