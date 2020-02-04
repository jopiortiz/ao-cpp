/******************************************************************************
    Copyright (C) 2002-2015 Argentum Online & Dakara Online Developers

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "Centinela.h"
#include "allheaders.h"
/* [(0, 'ATTRIBUTE'), (1, 'VB_Name'), (5, '='), (4, '"modCentinela"')] */
/* '***************************************************************** */
/* 'modCentinela.bas - ImperiumAO - v1.2 */
/* ' */
/* 'Funciónes de control para usuarios que se encuentran trabajando */
/* ' */

/* '***************************************************************** */
/* 'Augusto Rando(barrin@imperiumao.com.ar) */
/* '   ImperiumAO 1.2 */
/* '   - First Relase */
/* ' */
/* 'Juan Martín Sotuyo Dodero (juansotuyo@gmail.com) */
/* '   Alkon AO 0.11.5 */
/* '   - Small improvements and added logs to detect possible cheaters */
/* ' */
/* 'Juan Martín Sotuyo Dodero (juansotuyo@gmail.com) */
/* '   Alkon AO 0.12.0 */
/* '   - Added several messages to spam users until they reply */
/* ' */
/* 'ZaMa */
/* '   Alkon AO 0.13.0 */
/* '   - Added several paralel checks */
/* '***************************************************************** */

bool centinelaActivado;          				//Esta activado?

static const int NUM_CENTINELAS  = 5;       	//Cantidad de centinelas.
static const int NUM_NPC  = 16;     			//NpcNum del centinela.

static const int MAPA_EXPLOTAR = 15;     		//Numero de mapa en la qe se pinchan usuarios.
static const int X_EXPLOTAR = 50;        		//X
static const int Y_EXPLOTAR = 50;        		//Y

static const long LIMITE_TIEMPO  = 120000;  	//Tiempo limite (milisegundos), 2 minutos.
static const int CARCEL_TIEMPO = 5;         	//Minutos en la carcel
static const long REVISION_TIEMPO = 1800000;	//Tiempo de cada revision (milisegundos) 1.800.000 = 30 minutos (60 segundos * 30 minutos) * 1000 milisegundos

vb6::array<struct tCentinela> Centinela;

void CambiarEstado(int gmIndex) {
	/*
		Author: Unknown
		Last Modification: 13/11/2019 (Recox)

		* Cambia el estado del centinela.

		13/11/2019 Recox: La variable isCentinelaActivated tiene un nombre mas descriptivo
	*/

    // Lo cambiamos en la memoria.
    centinelaActivado = !centinelaActivado;

    // Lo cambiamos en el Server.ini
    WriteVar(GetIniPath("server.ini"), "INIT", "CentinelaAuditoriaTrabajoActivo", centinelaActivado ? "1" : "0");

    // Preparamos el aviso por consola.
    std::string message;
    message = UserList[gmIndex].Name + " cambio el estado del Centinela a " + (centinelaActivado ? " ACTIVADO." : " DESACTIVADO.");

    // Mandamos el aviso por consola.
    SendData(SendTarget::SendTarget_ToAll, 0, PrepareMessageConsoleMsg(message, FontTypeNames::FontTypeNames_FONTTYPE_CENTINELA));

    // Lo registramos en los logs.
    LogGM(UserList[gmIndex].Name, message);

}

void EnviarAUsuario(int Userindex, int CIndex) {

    // Envia centinela a un usuario.

    // Genera el codigo.
    Centinela[CIndex].CodigoCheck = vb6::CStr(rand() % 9999);

    // Spawnea.
    Centinela[CIndex].MiNpcIndex = SpawnNpc(NUM_NPC, DarPosicion(Userindex), true, false);

    // Setea el flag.
    Centinela[CIndex].Invocado = (Centinela[CIndex].MiNpcIndex != 0);

    // No spawnea, error !
    if (!Centinela[CIndex].Invocado) {
        Centinela[CIndex].CodigoCheck = "";
    }

    // Avisa al usuario sobre el char del centinela.
    AvisarUsuario(Userindex, CIndex);

    // Setea el tiempo.
	Centinela[CIndex].TiempoInicio = vb6::GetTickCount();

    // Setea UI del usuario
    Centinela[CIndex].RevisandoSlot = Userindex;

    // Setea los datos del user.
    UserList[Userindex].CentinelaUsuario.CentinelaCheck = false;     				// Por defecto, no ingreso la clave.
    UserList[Userindex].CentinelaUsuario.centinelaIndex = CIndex;                   // Setea el index del mismo.
    UserList[Userindex].CentinelaUsuario.Codigo = Centinela[CIndex].CodigoCheck;    // Setea el codigo.
    UserList[Userindex].CentinelaUsuario.Revisando = true;                          // Lo revisa un centinela.

}

void AvisarUsuarios() {

    // Envia la clave a los usuarios de los centinelas
    for (long i = 1; i <= NUM_CENTINELAS; i++) {

        // Si esta invocado.
        if (Centinela[i].Invocado) {

			// Avisa.
            AvisarUsuario(Centinela[i].RevisandoSlot, vb6::CByte(i));

        }
    };

}

void AvisarUsuario(int userSlot, int centinelaIndex, bool IngresoFallido = false) {

    // Avisa al usuario la clave..


    std::string DataSend;

    // Para avisar.
    if (!IngresoFallido) {

        // Paso la mitad de tiempo?
        if (vb6::GetTickCount() - Centinela[centinelaIndex].TiempoInicio > (LIMITE_TIEMPO / 2)) {
            // Prepara el paquete a enviar.
            DataSend = PrepareMessageChatOverHead("CONTROL DE MACRO INASISTIDO, Debes escribir /CENTINELA " + Centinela[centinelaIndex].CodigoCheck + " En menos de 2 minutos.", Npclist[Centinela[centinelaIndex].MiNpcIndex].Char.CharIndex, vbYellow);
        } else {
            DataSend = PrepareMessageChatOverHead("CONTROL DE MACRO INASISTIDO, Tienes menos de un minuto para escribir /CENTINELA " + Centinela[centinelaIndex].CodigoCheck + ".", Npclist[Centinela[centinelaIndex].MiNpcIndex].Char.CharIndex, vbYellow);
        }

    } else {
        DataSend = PrepareMessageChatOverHead("CONTROL DE MACRO INASISTIDO, El codigo ingresado NO es correcto, debes escribir : /CENTINELA " + Centinela[centinelaIndex].CodigoCheck + ".", Npclist[Centinela[centinelaIndex].MiNpcIndex].Char.CharIndex, vbYellow);

    }

    // Envia.
    EnviarDatosASlot(userSlot, DataSend);

}

void ChekearUsuarios() {

    int CIndex;

    for (long LoopC = 1; LoopC <= LastUser; LoopC++) {

            // Lo revisa el centinela?
            if (UserList[LoopC].CentinelaUsuario.Revisando) {
                TiempoUsuario(vb6::CInt(LoopC));

            } else {

                // Esta trabajando?
                if (UserList[LoopC].Counters.Trabajando != 0) {

                    // Si todavia no lo revisaron o si paso mas del tiempo sin revisar, vuelve a enviar.
                    if (!UserList[LoopC].CentinelaUsuario.CentinelaCheck || ((vb6::GetTickCount() - UserList[LoopC].CentinelaUsuario.UltimaRevision) > REVISION_TIEMPO)) {

                        // Busca un slot para centinela y se lo envia.
                        CIndex = ProximoCentinela();

                        // Si hay slot
                        if (CIndex != 0) {

                            // Envia
                            EnviarAUsuario(vb6::CInt(LoopC), CIndex);

                        }

                    }

                }

            }

    }

}

void IngresaClave(int UserIndex, std::string Clave) {

    // Checkea la clave que ingreso el usuario.

    Clave = vb6::UCase(Clave);

    int centinelaIndex;

    centinelaIndex = UserList[UserIndex].CentinelaUsuario.centinelaIndex;

    // No tiene centinela.
    if (!centinelaIndex != 0) { return; }

    // No esta revisandolo.
    if (!UserList[UserIndex].CentinelaUsuario.Revisando) { return; }

    // Checkea el codigo
    if (CheckCodigo(Clave, centinelaIndex)) {

        // Quita el centinela.
        AprobarUsuario(UserIndex, centinelaIndex);

    } else {

        // Avisa.
        AvisarUsuario(UserIndex, centinelaIndex, true);

    }

}

void AprobarUsuario(int UserIndex, int CIndex) {

    // Aprueba el control de un usuario.

    // Quita el char.
    LimpiarIndice(UserList[UserIndex].CentinelaUsuario.centinelaIndex);

    UserList[UserIndex].CentinelaUsuario.CentinelaCheck = true;
    UserList[UserIndex].CentinelaUsuario.centinelaIndex = 0;
    UserList[UserIndex].CentinelaUsuario.Codigo = "";
    UserList[UserIndex].CentinelaUsuario.Revisando = false;
    UserList[UserIndex].CentinelaUsuario.UltimaRevision = vb6::GetTickCount();

    WriteConsoleMsg(UserIndex, "El control ha finalizado.", FontTypeNames::FontTypeNames_FONTTYPE_DIOS);

}

void LimpiarIndice(int centinelaIndex) {

    // Limpia un slot.

    Centinela[centinelaIndex].Invocado = false;
    Centinela[centinelaIndex].CodigoCheck = "";
    Centinela[centinelaIndex].RevisandoSlot = 0;
    Centinela[centinelaIndex].TiempoInicio = 0;

    // Estaba el char?
    if (Centinela[centinelaIndex].MiNpcIndex != 0) {
        QuitarNPC(Centinela[centinelaIndex].MiNpcIndex);
    }

}

void TiempoUsuario(int UserIndex) {

    // Checkea el tiempo para contestar de un usuario.

    int centinelaIndex;

    centinelaIndex = UserList[UserIndex].CentinelaUsuario.centinelaIndex;

    // No hay indice ! WTF XD
    if (!centinelaIndex != 0) { return; }

    // Acabo el tiempo y no ingreso la clave.
    if ((vb6::GetTickCount() - Centinela[centinelaIndex].TiempoInicio) > LIMITE_TIEMPO) {
        UsuarioInActivo(UserIndex);
    }

}

void UsuarioInActivo(int Userindex) {

    // No contesto el usuario, se lo pena.

    // Telep al mapa.
    WarpUserChar(Userindex, MAPA_EXPLOTAR, X_EXPLOTAR, Y_EXPLOTAR, true);


    // No creo que tirar los items sea justo, con encarcelarlo y matarlo es mas que suficiente. (Recox)
    // Aparte de que si muere desaparecen los items...

    // Muere.
    // UserDie(Userindex)

    // Tira los items.
    // TirarTodosLosItems(Userindex)

    // Lo encarcela.
    Encarcelar(Userindex, CARCEL_TIEMPO, "El centinela");

    // Borra el centinela.
    if (UserList[Userindex].CentinelaUsuario.centinelaIndex != 0) {
        LimpiarIndice(UserList[Userindex].CentinelaUsuario.centinelaIndex);
    }

    // Deja un mensaje.
    WriteConsoleMsg(Userindex, "El centinela te ha ejecutado y encarcelado por Macro Inasistido.", FontTypeNames::FontTypeNames_FONTTYPE_DIOS);

    // Limpia el tipo del usuario.
    struct CentinelaUser clearType;

    UserList[Userindex].CentinelaUsuario = clearType;

    UserList[Userindex].CentinelaUsuario.CentinelaCheck = true;

}

WorldPos DarPosicion(int UserIndex) {

    // Devuelve la posicion para spawnear al centinela.
	int X = UserList[UserIndex].Pos.X + 1;
	int Y = UserList[UserIndex].Pos.Y;

	return WorldPos(Position(X, Y));

}

int ProximoCentinela() {

    // Devuelve un slot para un centinela.
    for (long i = 1; i <= NUM_CENTINELAS; i++) {

        // Si no esta invocado.
        if (!Centinela[i].Invocado) {

			// Devuelve el slot
            return vb6::CByte(i);

        }

    };

    return 0;

}

bool CheckCodigo(std::string Ingresada, int CIndex) {

    // Devuelve si el codigo es correcto.
    return !(Ingresada != Centinela[CIndex].CodigoCheck);

}
