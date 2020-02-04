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

/*  AUTOMATICALLY CONVERTED FILE  */
#ifndef MODCENTINELA_H
#define MODCENTINELA_H
#include <vector>
#include <memory>
#include <string>
#include <cstdlib>
#include "vb6compat.h"
#include "enums.h"

 struct tCentinela {
    /* Index of centinela en el servidor */
    int MiNpcIndex;

    /* Esta invocado? */
    bool Invocado;

    /* UI Del usuario */
    int RevisandoSlot;

    /* Desde que empezo el chekeo al usuario */
    int TiempoInicio;

    /* Codigo que debe ingresar el usuario */
    std::string CodigoCheck;
};

extern bool centinelaActivado;

/* 'Guardo cuando voy a resetear a la lista de usuarios del centinela */

extern const int NRO_CENTINELA;
extern vb6::array<struct tCentinela> Centinela;

 void CambiarEstado(int gmIndex);

 void EnviarAUsuario(int Userindex, int CIndex);

 void AvisarUsuarios();

 void AvisarUsuario(int userSlot, int centinelaIndex, bool IngresoFallido = false);

 void ChekearUsuarios();

 void IngresaClave(int UserIndex, std::string Clave);

 void AprobarUsuario(int UserIndex, int CIndex);

 void LimpiarIndice(int centinelaIndex);

 void TiempoUsuario(int UserIndex);

 void UsuarioInActivo(int Userindex);

 std::string GenerarClave();

 WorldPos DarPosicion(int UserIndex);

 int ProximoCentinela();

 bool CheckCodigo(std::string Ingresada, int CIndex);

#endif
