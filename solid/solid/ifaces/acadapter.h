/*
    Copyright 2006 Kevin Ottens <ervin@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SOLID_IFACES_ACADAPTER_H
#define SOLID_IFACES_ACADAPTER_H

#include <solid/ifaces/deviceinterface.h>

namespace Solid
{
namespace Ifaces
{
    /**
     * This device interface is available on AC adapters.
     */
    class AcAdapter : virtual public DeviceInterface
    {
    public:
        /**
         * Destroys an AcAdapter object.
         */
        virtual ~AcAdapter();


        /**
         * Indicates if this AC adapter is plugged.
         *
         * @return true if the adapter is plugged, false otherwise
         */
        virtual bool isPlugged() const = 0;

    protected:
    //Q_SIGNALS:
        /**
         * This signal is emitted when the AC adapter is plugged or unplugged.
         *
         * @param newState true if the AC adapter is plugged is mounted, false otherwise
         * @param udi the UDI of the AC adapter with the changed plugging state
         */
        virtual void plugStateChanged(bool newState, const QString &udi) = 0;
    };
}
}

Q_DECLARE_INTERFACE(Solid::Ifaces::AcAdapter, "org.kde.Solid.Ifaces.AcAdapter/0.1")

#endif
