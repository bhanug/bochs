/////////////////////////////////////////////////////////////////////////
// $Id: ioapic.h,v 1.36 2010-02-28 14:52:17 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2009  The Bochs Project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_DEVICES_IOAPIC_H
#define BX_DEVICES_IOAPIC_H

#if BX_SUPPORT_APIC

extern int apic_bus_deliver_lowest_priority(Bit8u vector, Bit8u dest, bx_bool trig_mode, bx_bool broadcast);
extern int apic_bus_deliver_interrupt(Bit8u vector, Bit8u dest, Bit8u delivery_mode, bx_bool logical_dest, bx_bool level, bx_bool trig_mode);
extern int apic_bus_broadcast_interrupt(Bit8u vector, Bit8u delivery_mode, bx_bool trig_mode, int exclude_cpu);

#define BX_IOAPIC_NUM_PINS   (0x18)

// use the same version as 82093 IOAPIC (0x00170011)
#define BX_IOAPIC_VERSION_ID (((BX_IOAPIC_NUM_PINS - 1) << 16) | 0x11)

extern Bit32u apic_id_mask;

class bx_io_redirect_entry_t {
  Bit32u hi, lo;

public:
  bx_io_redirect_entry_t(): hi(0), lo(0x10000) {}

  Bit8u destination() const { return (Bit8u)((hi >> 24) & apic_id_mask); }
  bx_bool is_masked() const { return (bx_bool)((lo >> 16) & 1); }
  Bit8u trigger_mode() const { return (Bit8u)((lo >> 15) & 1); }
  bx_bool remote_irr() const { return (bx_bool)((lo >> 14) & 1); }
  Bit8u pin_polarity() const { return (Bit8u)((lo >> 13) & 1); }
  bx_bool delivery_status() const { return (bx_bool)((lo >> 12) & 1); }
  Bit8u destination_mode() const { return (Bit8u)((lo >> 11) & 1); }
  Bit8u delivery_mode() const { return (Bit8u)((lo >> 8) & 7); }
  Bit8u vector() const { return (Bit8u)(lo & 0xff); }

  void set_delivery_status() { lo |= (1<<12); }
  void clear_delivery_status() { lo &= ~(1<<12); }
  void set_remote_irr() { lo |= (1<<14); }
  void clear_remote_irr() { lo &= ~(1<<14); }

  Bit32u get_lo_part () const { return lo; }
  Bit32u get_hi_part () const  { return hi; }
  void set_lo_part (Bit32u val_lo_part) {
    // keep high 32 bits of value, replace low 32, ignore R/O bits
    lo = val_lo_part & 0xffffafff;
  }
  void set_hi_part (Bit32u val_hi_part) {
    // keep low 32 bits of value, replace high 32
    hi = val_hi_part;
  }
  void sprintf_self(char *buf);
  void register_state(bx_param_c *parent);
};

class bx_ioapic_c : public bx_ioapic_stub_c {
public:
  bx_ioapic_c();
  virtual ~bx_ioapic_c() {}
  virtual void init();
  virtual void reset(unsigned type);
  virtual void register_state(void);

  virtual void receive_eoi(Bit8u vector);
  virtual void set_irq_level(Bit8u int_in, bx_bool level);

  Bit32u read_aligned(bx_phy_address address);
  void write_aligned(bx_phy_address address, Bit32u data);

private:
  bx_phy_address get_base(void) const { return base_addr; }
  void set_id(Bit32u new_id) { id = new_id; }
  Bit32u get_id() const { return id; }

  void service_ioapic(void);

  bx_phy_address base_addr;
  Bit32u id;

  Bit32u ioregsel;    // selects between various registers
  Bit32u intin;
  // interrupt request bitmask, not visible from the outside.  Bits in the
  // irr are set when trigger_irq is called, and cleared when the interrupt
  // is delivered to the processor.  If an interrupt is masked, the irr
  // will still be set but delivery will not occur until it is unmasked.
  // It's not clear if this is how the real device works.
  Bit32u irr;

  bx_io_redirect_entry_t ioredtbl[BX_IOAPIC_NUM_PINS];  // table of redirections
};

#endif

#endif
