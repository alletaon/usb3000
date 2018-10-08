# coding=UTF8
from ctypes import *
from time import sleep

# --------------------------- КОНСТАНТЫ ---------------------------------------
# адрес начала сегмента блока данных в памяти программ DSP
VarsBaseAddress_USB3000 = 0x30
# тактовая частота работы DSP в кГц
DSP_CLOCK_OUT_USB3000 = 72000

class Usb3000:
	def __init__(self):
		self.lib = windll.LoadLibrary('./wrRtusbapi.dll')
		self.lib.GetDllVersion_Rtusbapi.restype = c_ushort
		self.lib.CreateInstance_Rtusbapi.argtypes = (c_char_p, )
		self.lib.CreateInstance_Rtusbapi.restype = c_void_p
		self.lib.OpenDevice_Usb3000.argtypes = (c_void_p, c_ushort)
		self.lib.OpenDevice_Usb3000.restype = c_bool
		self.lib.ENABLE_TTL_OUT_Usb3000.argtypes = (c_void_p, c_bool)
		self.lib.ENABLE_TTL_OUT_Usb3000.restype = c_bool
		self.lib.TTL_IN_Usb3000.argtypes = (c_void_p, POINTER(c_ushort))
		self.lib.TTL_IN_Usb3000.restype = c_bool
		self.lib.TTL_OUT_Usb3000.argtypes = (c_void_p, c_ushort)
		self.lib.TTL_OUT_Usb3000.restype = c_bool
		
	def get_DLL_version(self):
		return int(self.lib.GetDllVersion_Rtusbapi())
		
	def create_instace(self):
		arg = create_string_buffer(b'usb3000') 
		return self.lib.CreateInstance_Rtusbapi(c_char_p(b'usb3000'))
		
	def open_device(self, instance, slot):
		return self.lib.OpenDevice_Usb3000(instance, c_ushort(slot))
		
	def enable_ttl_out(self, instance, enable):
		self.lib.ENABLE_TTL_OUT_Usb3000(instance, c_bool(enable))
		
	def get_ttl_inputs(self, instance):
		result = pointer(c_ushort(0))
		self.lib.TTL_IN_Usb3000(instance, result)
		return result.contents.value
		
	def set_ttl_out(self, instance, outputs):
		self.lib.TTL_OUT_Usb3000(instance, c_ushort(outputs))
		
# структура, задающая режимы ввода данных для модуля USB-3000
class Usb3000InputParams:
	pass
	
# структура, задающая режимы вывода данных для модуля USB-3000
class Usb3000OutputParams:
	pass
	
# структура пользовательского ППЗУ
class Usb3000Flash:
	pass
# структура, содержащая информацию о версии драйвера DSP	
class Usb3000DSPInfo:
	pass
	
if __name__ == '__main__':
	dev = Usb3000()
	print(dev.get_DLL_version())
	# получим указатель на интерфейс модуля USB3000
	pModule = dev.create_instace()
	# попробуем обнаружить модуль USB3000 в первых 127 виртуальных слотах
	i = 0
	while i < 128:
		if dev.open_device(pModule, i):
			break
		i += 1
	if i == 128:
		raise Exception("Can't find module USB3000 in first 127 virtual slots!")
	
	#LOAD_DSP_Usb3000(pModule, NULL)
	#MODULE_TEST_Usb3000(pModule)
	#GET_DSP_INFO()
	#GET_FLASH()
	dev.enable_ttl_out(pModule, True)
	inputs = dev.get_ttl_inputs(pModule)
	print('inputs:{0}'.format(inputs))
	