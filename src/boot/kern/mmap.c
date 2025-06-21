#include "mmap.h"
#include "bios.h"
#include "sort.h"
#include "string.h"
#include "text.h"
#include "types.h"

#define MAX_MM_ENTRIES 128
#define MAX_MM_EVENTS  (MAX_MM_ENTRIES * 2)

typedef enum mm_type
{
  MM_TYPE_FREE = 0x1,
  MM_TYPE_RESERVED = 0x2,
  MM_TYPE_ACPI = 0x3,
  MM_TYPE_ACPI_NVS = 0x4,
  MM_TYPE_BAD = 0x5
} __attribute__ ((packed)) mm_type;

typedef struct mm_entry_t
{
  root_u64 base;
  root_u64 length;
  root_u32 type;
  root_u32 acpi_attribs;
} mm_entry_t;

typedef enum mm_entry_event_type_t
{
  MM_ENTRY_EVENT_TYPE_START = 0,
  MM_ENTRY_EVENT_TYPE_END = 1
} mm_entry_event_type_t;

typedef struct mm_entry_event_t
{
  mm_entry_t *entry;
  root_u64 address;
} mm_entry_event_t;

static root_u16 num_mm_entries;
static mm_entry_t mm_entries[MAX_MM_ENTRIES];

static int
root_event_cmp (const void *e1, const void *e2)
{
  const mm_entry_event_t *evt1 = e1, *evt2 = e2;
  if (evt1->address != evt2->address)
    return evt1->address < evt2->address ? -1 : 1;
  return (evt1->address == evt1->entry->base)
         - (evt2->address == evt2->entry->base);
}

static int
root_fixup_mmap (void)
{
  root_u64 last_addr = 0;
  root_u32 last_type = 0;
  root_u16 num_new_entries = 0, num_events = 0, num_overlap_entries = 0;
  mm_entry_t new_entries[MAX_MM_ENTRIES];
  mm_entry_event_t events[MAX_MM_EVENTS];
  mm_entry_t *overlap_list[MAX_MM_ENTRIES];
  if (num_mm_entries <= 1)
    return 0;
  for (int i = 0; i < num_mm_entries; i++)
    {
      mm_entry_t *entry = mm_entries + i;
      if (entry->base + entry->length <= entry->base)
        continue;
      events[num_events++]
          = (mm_entry_event_t){ .entry = entry, .address = entry->base };
      events[num_events++]
          = (mm_entry_event_t){ .entry = entry,
                                .address = entry->base + entry->length };
    }
  root_ssort (events, num_events, sizeof (mm_entry_event_t), root_event_cmp);
  for (int i = 0; i < num_events; i++)
    {
      root_u32 cur_type = 0;
      mm_entry_event_t event = events[i];
      mm_entry_t *entry = event.entry;
      if (event.address == entry->base)
        overlap_list[num_overlap_entries++] = entry;
      else
        {
          for (int j = 0; j < num_overlap_entries; j++)
            {
              mm_entry_t *cur_entry = overlap_list[j];
              if (cur_entry == entry)
                {
                  overlap_list[j] = overlap_list[num_overlap_entries - 1];
                  break;
                }
            }
          num_overlap_entries--;
        }
      for (int j = 0; j < num_overlap_entries; j++)
        {
          mm_entry_t *cur_entry = overlap_list[j];
          if (cur_entry->type > cur_type)
            cur_type = cur_entry->type;
        }
      if (last_type != cur_type)
        {
          mm_entry_t *new_entry = new_entries + num_new_entries;
          if (last_type != 0)
            {
              new_entry->length = event.address - last_addr;
              if (new_entry->length > 0 && ++num_new_entries >= MAX_MM_ENTRIES)
                return -1;
            }
          if (cur_type != 0)
            {
              new_entry->base = event.address;
              new_entry->type = cur_type;
              new_entry->acpi_attribs = entry->acpi_attribs;
              last_addr = event.address;
            }
          last_type = cur_type;
        }
    }
  root_memcpy (mm_entries, new_entries, sizeof (mm_entry_t) * num_new_entries);
  num_mm_entries = num_new_entries;
  return 0;
}

static int
mm_get_entry (root_bios_args_t *args, mm_entry_t *entry)
{
  root_memset (entry, 0, sizeof (mm_entry_t));
  args->eax = 0xE820;
  args->ecx = 20;
  args->edx = 0x534D4150;
  args->es = root_get_segment ((uintptr_t) entry);
  args->edi = root_get_offset ((uintptr_t) entry);
  root_bios_interrupt (0x15, args);
  if (args->flags & ROOT_BIOS_CARRY_FLAG)
    return -1;
  switch (entry->type)
    {
    case MM_TYPE_FREE:
    case MM_TYPE_RESERVED:
    case MM_TYPE_ACPI:
    case MM_TYPE_ACPI_NVS:
    case MM_TYPE_BAD:
      break;
    default:
      entry->type = MM_TYPE_RESERVED;
      break;
    }
  return 0;
}

int
root_init_mmap (void)
{
  root_bios_args_t args;
  int r;
  num_mm_entries = 0;
  args.ebx = 0;
  do
    {
      r = mm_get_entry (&args, mm_entries + num_mm_entries);
      if (r)
        return r;
      if (++num_mm_entries >= MAX_MM_ENTRIES)
        break;
    }
  while (args.eax == 0x534D4150 && args.ebx != 0x0);
  r = root_fixup_mmap ();
  if (r)
    {
      root_printf ("fixup_mmap failed\n");
      return r;
    }
  for (int i = 0; i < num_mm_entries; i++)
    {
      mm_entry_t *e = mm_entries + i;
      root_printf ("Region [Address=%#llX : %#llX, Type=%u]\n", e->base,
                   e->base + e->length, (unsigned int) e->type);
    }
  return 0;
}
