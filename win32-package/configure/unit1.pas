unit Unit1; 

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, LResources, Forms, Controls, Graphics, Dialogs, StdCtrls,
  Buttons;

type

  { TForm1 }

  TForm1 = class(TForm)
    bCancel: TButton;
    bSave: TButton;
    recv_interval: TComboBox;
    Label6: TLabel;
    idle_interval: TEdit;
    idler: TCheckBox;
    convert_smiles: TCheckBox;
    show_advert: TCheckBox;
    voiced_girls: TCheckBox;
    GroupBox1: TGroupBox;
    GroupBox2: TGroupBox;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    nick1: TEdit;
    password1: TEdit;
    procedure Form1Create(Sender: TObject);
    procedure bCancelClick(Sender: TObject);
    procedure bSaveClick(Sender: TObject);
    procedure idlerChange(Sender: TObject);
  private
    { private declarations }
  public
    { public declarations }
  end; 

var
  Form1: TForm1; 

implementation

{ TForm1 }

procedure TForm1.bCancelClick(Sender: TObject);
begin
  Form1.Close;
end;

procedure TForm1.Form1Create(Sender: TObject);
begin
  if (FileExists('xchat.conf')) then begin
     ShowMessage(
      'Byly nalezeny soubory s konfiguraci Xchat. '+
      'To pravdepodobne znamena ze provadite upgrade. '+
      'Pokud nastaveni nyni ulozite, dojde k prepsani '+
      'konfigurace programu Xchat. Doporucujeme '+
      'kliknout na tlacitko Zrusit'
     );
  end;
end;

procedure TForm1.bSaveClick(Sender: TObject);
var f:TextFile;
    a:integer;
begin
(* Overeni zadani *)
if (nick1.Text = '') then begin
   ShowMessage('Musíte zadat nick!');
   exit;
end;
if (password1.Text = '') then begin
   ShowMessage('Musite zadat heslo!');
   exit;
end;

try
   a := StrToInt(recv_interval.Text);
except on Exception : EConvertError do begin
   ShowMessage('Obnova skla musi byt císlo v sekundach!');
   exit;
   end;
end;

if (idler.Checked) then
   try
      a := StrToInt(idle_interval.Text);
   except on Exception : EConvertError do begin
      ShowMessage('Interval udrzovace musi byt císlo v sekundach!');
      exit;
   end;
end;

(* ServList *)
  AssignFile(f, 'xchat.conf');
  ReWrite(f);
  Write(f, 'version = 2.4.5'+#10);
  Write(f, 'auto_save = 1'+#10);
  Write(f, 'auto_save_url = 0'+#10);
  Write(f, 'away_auto_unmark = 0'+#10);
  Write(f, 'away_reason ='+#10);
  Write(f, 'away_show_message = 0'+#10);
  Write(f, 'away_show_once = 1'+#10);
  Write(f, 'away_size_max = 300'+#10);
  Write(f, 'away_timeout = 60'+#10);
  Write(f, 'away_track = 1'+#10);
  Write(f, 'completion_amount = 5'+#10);
  Write(f, 'completion_auto = 0'+#10);
  Write(f, 'completion_suffix = :'+#10);
  Write(f, 'dcc_auto_chat = 0'+#10);
  Write(f, 'dcc_auto_resume = 1'+#10);
  Write(f, 'dcc_auto_send = 0'+#10);
  Write(f, 'dcc_blocksize = 1024'+#10);
  Write(f, 'dcc_completed_dir ='+#10);
  Write(f, 'dcc_dir = C:\'+#10);
  Write(f, 'dcc_fast_send = 1'+#10);
  Write(f, 'dcc_global_max_get_cps = 0'+#10);
  Write(f, 'dcc_global_max_send_cps = 0'+#10);
  Write(f, 'dcc_ip ='+#10);
  Write(f, 'dcc_ip_from_server = 0'+#10);
  Write(f, 'dcc_max_get_cps = 0'+#10);
  Write(f, 'dcc_max_send_cps = 0'+#10);
  Write(f, 'dcc_permissions = 384'+#10);
  Write(f, 'dcc_port_first = 0'+#10);
  Write(f, 'dcc_port_last = 0'+#10);
  Write(f, 'dcc_remove = 0'+#10);
  Write(f, 'dcc_save_nick = 0'+#10);
  Write(f, 'dcc_send_fillspaces = 1'+#10);
  Write(f, 'dcc_stall_timeout = 60'+#10);
  Write(f, 'dcc_timeout = 180'+#10);
  Write(f, 'dnsprogram = host'+#10);
  Write(f, 'flood_ctcp_num = 5'+#10);
  Write(f, 'flood_ctcp_time = 30'+#10);
  Write(f, 'flood_msg_num = 5'+#10);
  Write(f, 'flood_msg_time = 30'+#10);
  Write(f, 'gui_auto_open_chat = 1'+#10);
  Write(f, 'gui_auto_open_dialog = 1'+#10);
  Write(f, 'gui_auto_open_recv = 1'+#10);
  Write(f, 'gui_auto_open_send = 1'+#10);
  Write(f, 'gui_dialog_height = 256'+#10);
  Write(f, 'gui_dialog_left = 0'+#10);
  Write(f, 'gui_dialog_top = 0'+#10);
  Write(f, 'gui_dialog_width = 500'+#10);
  Write(f, 'gui_hide_menu = 0'+#10);
  Write(f, 'gui_input_style = 0'+#10);
  Write(f, 'gui_lagometer = 1'+#10);
  Write(f, 'gui_mode_buttons = 0'+#10);
  Write(f, 'gui_slist_select = 0'+#10);
  Write(f, 'gui_slist_skip = 1'+#10);
  Write(f, 'gui_throttlemeter = 1'+#10);
  Write(f, 'gui_topicbar = 1'+#10);
  Write(f, 'gui_ulist_buttons = 0'+#10);
  Write(f, 'gui_ulist_doubleclick = QUOTE WHOIS %s %s'+#10);
  Write(f, 'gui_ulist_hide = 0'+#10);
  Write(f, 'gui_ulist_pos = 0'+#10);
  Write(f, 'gui_ulist_resizable = 1'+#10);
  Write(f, 'gui_ulist_show_hosts = 0'+#10);
  Write(f, 'gui_ulist_sort = 0'+#10);
  Write(f, 'gui_ulist_style = 0'+#10);
  Write(f, 'gui_win_height = 544'+#10);
  Write(f, 'gui_win_left = -4'+#10);
  Write(f, 'gui_win_save = 1'+#10);
  Write(f, 'gui_win_state = 1'+#10);
  Write(f, 'gui_win_top = -4'+#10);
  Write(f, 'gui_win_width = 800'+#10);
  Write(f, 'identd = 0'+#10);
  Write(f, 'input_beep_chans = 0'+#10);
  Write(f, 'input_beep_hilight = 0'+#10);
  Write(f, 'input_beep_msg = 0'+#10);
  Write(f, 'input_command_char = /'+#10);
  Write(f, 'input_filter_beep = 0'+#10);
  Write(f, 'input_flash_hilight = 1'+#10);
  Write(f, 'input_fudge_snotice = 0'+#10);
  Write(f, 'input_perc_ascii = 0'+#10);
  Write(f, 'input_perc_color = 0'+#10);
  Write(f, 'irc_auto_rejoin = 0'+#10);
  Write(f, 'irc_ban_type = 2'+#10);
  Write(f, 'irc_conf_mode = 0'+#10);
  Write(f, 'irc_extra_hilight ='+#10);
  Write(f, 'irc_hide_version = 0'+#10);
  Write(f, 'irc_id_ytext ='+#10);
  Write(f, 'irc_id_ntext ='+#10);
  Write(f, 'irc_invisible = 0'+#10);
  Write(f, 'irc_logging = 0'+#10);
  Write(f, 'irc_logmask = %n-%c.log'+#10);
  Write(f, 'irc_nick_hilight ='+#10);
  Write(f, 'irc_nick1 = '+nick1.Text+#10);
  Write(f, 'irc_nick2 ='+#10);
  Write(f, 'irc_nick3 ='+#10);
  Write(f, 'irc_no_hilight ='+#10);
  Write(f, 'irc_part_reason = Leaving'+#10);
  Write(f, 'irc_quit_reason = Leaving'+#10);
  Write(f, 'irc_raw_modes = 0'+#10);
  Write(f, 'irc_real_name = '+nick1.Text+#10);
  Write(f, 'irc_servernotice = 0'+#10);
  Write(f, 'irc_skip_motd = 0'+#10);
  Write(f, 'irc_user_name = '+nick1.Text+#10);
  Write(f, 'irc_wallops = 0'+#10);
  Write(f, 'irc_who_join = 1'+#10);
  Write(f, 'irc_whois_front = 0'+#10);
  Write(f, 'net_auto_reconnect = 1'+#10);
  Write(f, 'net_auto_reconnectonfail = 0'+#10);
  Write(f, 'net_bind_host ='+#10);
  Write(f, 'net_ping_timeout = 0'+#10);
  Write(f, 'net_proxy_auth = 0'+#10);
  Write(f, 'net_proxy_host ='+#10);
  Write(f, 'net_proxy_pass ='+#10);
  Write(f, 'net_proxy_port = 0'+#10);
  Write(f, 'net_proxy_type = 0'+#10);
  Write(f, 'net_proxy_user ='+#10);
  Write(f, 'net_reconnect_delay = 10'+#10);
  Write(f, 'net_throttle = 1'+#10);
  Write(f, 'notify_timeout = 15'+#10);
  Write(f, 'notify_whois_online = 0'+#10);
  Write(f, 'perl_warnings = 0'+#10);
  Write(f, 'sound_command ='+#10);
  Write(f, 'sound_dir = ./sounds'+#10);
  Write(f, 'stamp_log = 1'+#10);
  Write(f, 'stamp_log_format = %b %d %H:%M:%S'+#10);
  Write(f, 'stamp_text = 0'+#10);
  Write(f, 'stamp_text_format = [%H:%M:%s]'+#10);
  Write(f, 'tab_chans = 1'+#10);
  Write(f, 'tab_dialogs = 1'+#10);
  Write(f, 'tab_dnd = 0'+#10);
  Write(f, 'tab_new_to_front = 2'+#10);
  Write(f, 'tab_notices = 0'+#10);
  Write(f, 'tab_position = 0'+#10);
  Write(f, 'tab_server = 1'+#10);
  Write(f, 'tab_small = 0'+#10);
  Write(f, 'tab_sort = 1'+#10);
  Write(f, 'tab_trunc = 20'+#10);
  Write(f, 'tab_utils = 0'+#10);
  Write(f, 'text_background ='+#10);
  Write(f, 'text_color_nicks = 1'+#10);
  Write(f, 'text_font = Monospace 9'+#10);
  Write(f, 'text_indent = 1'+#10);
  Write(f, 'text_max_indent = 256'+#10);
  Write(f, 'text_max_lines = 300'+#10);
  Write(f, 'text_show_marker = 1'+#10);
  Write(f, 'text_show_sep = 1'+#10);
  Write(f, 'text_stripcolor = 0'+#10);
  Write(f, 'text_thin_sep = 1'+#10);
  Write(f, 'text_tint = 0'+#10);
  Write(f, 'text_tint_blue = 195'+#10);
  Write(f, 'text_tint_green = 195'+#10);
  Write(f, 'text_tint_red = 195'+#10);
  Write(f, 'text_transparent = 0'+#10);
  Write(f, 'text_wordwrap = 1'+#10);
  CloseFile(f);

(* ServList *)
  AssignFile(f, 'servlist_.conf');
  ReWrite(f);
  Write(f,'v=2.4.5'+#10);
  Write(f,'N=xchat-bttrw'+#10);
  Write(f,'P='+password1.Text+#10);
  Write(f,'E=UTF-8'+#10);
  Write(f,'C=CONN'+#10);
  Write(f,'F=10'+#10);
  Write(f,'D=0'+#10);
  Write(f,'S=localhost/6669'+#10);
  CloseFile(f);

(* Commands *)
  AssignFile(f, 'commands.conf');
  ReWrite(f);
  Write(f,'NAME CONN'+#10);
  if (convert_smiles.Checked) then
     Write(f,'CMD quote set convert_smiles 1'+#10)
  else
     Write(f,'CMD quote set convert_smiles 0'+#10);
  Write(f,#10);

  Write(f,'NAME CONN'+#10);
  if (show_advert.Checked) then
     Write(f,'CMD quote set show_advert 1'+#10)
  else
     Write(f,'CMD quote set show_advert 0'+#10);
  Write(f,#10);

  Write(f,'NAME CONN'+#10);
  if (voiced_girls.Checked) then
     Write(f,'CMD quote set voiced_girls 1'+#10)
  else
     Write(f,'CMD quote set voiced_girls 0'+#10);
  Write(f,#10);

  if (idler.Checked) then begin
     Write(f,'NAME CONN'+#10);
     Write(f,'CMD quote set idle_interval '+idle_interval.text+#10);
  end;
  Write(f,#10);

  Write(f,'NAME CONN'+#10);
  Write(f,'CMD quote set recv_interval '+recv_interval.Text+#10);

  CloseFile(f);

  ShowMessage('Nastaveni bylo (snad) ulozeno');

  Form1.Close;
end;

procedure TForm1.idlerChange(Sender: TObject);
begin
  if (idler.Checked) then begin
     idle_interval.Enabled:=true;
     ShowMessage('Varovani: Pouzivanim udrzovace, nemate-li to explicitne povoleno, porusujete pravidla Xchat.cz');
  end else
     idle_interval.Enabled:=false;
end;

initialization
  {$I unit1.lrs}

end.

