Object subclass: MockA [    
    method [
        ^#methodMockA    	    		 
	]
	
	callNoNamespace [
		^self method
	]
	
	callNamespaceA [
		<namespace: #A>
		^self method
	]
	
	callNamespaceB [
		<namespace: #B>
		^self method
	]
		 										
]	

MockA subclass: MockB [
    method [
    	<namespace: #A>	
        ^#methodMockB    	    		 
	]
]

MockB subclass: MockC [
    method [
    	<namespace: #B>	
        ^#methodMockC    	    		 
	]
	
]



CompiledMethod extend [
	namespace [ ^(self attributeAt: #namespace: ifAbsent:[^nil]) argument	]	
]

Object subclass: NamespacingBinder [
	bind: methods forMethod: senderMethod [
		| methodsInNamespace |
		'binding...' print. senderMethod namespace printNl.		
		methodsInNamespace := 
			methods select:[:mth|mth namespace = senderMethod namespace].			
		"try nil as fallback namespace"
		methodsInNamespace isEmpty ifTrue:[
		methodsInNamespace := 
			methods select:[:mth|mth namespace = nil]].
		
		^methodsInNamespace isEmpty 
			ifTrue:[nil]
			ifFalse:[methodsInNamespace first]
							
	]		
]

TestCase subclass: MOPMethodNamespacesTest [
	| mockA mockB mockC |
	
	setUp [		
		mockA := MockA new.
		mockB := MockB new.
		mockC := MockC new.
		Behavior flushCache.
		'==' printNl.
	]
	
	test_01 [
		self assert: mockA callNoNamespace == #methodMockA.
		self assert: mockB callNoNamespace == #methodMockB.
		self assert: mockC callNoNamespace == #methodMockC.
		
		self assert: mockA callNamespaceA == #methodMockA.
		self assert: mockB callNamespaceA == #methodMockB.
		self assert: mockC callNamespaceA == #methodMockC.
		
		self assert: mockA callNamespaceB == #methodMockA.
		self assert: mockB callNamespaceB == #methodMockB.
		self assert: mockC callNamespaceB == #methodMockC.
		
		(MockA >> #callNoNamespace) descriptor binder: NamespacingBinder new.
		(MockA >> #callNamespaceA) descriptor binder: NamespacingBinder new.
		(MockA >> #callNamespaceB) descriptor binder: NamespacingBinder new. 
		
		self assert: mockA callNoNamespace == #methodMockA.		
		self assert: mockB callNoNamespace == #methodMockA.	
		self assert: mockC callNoNamespace == #methodMockA.		
				
		self assert:  mockA callNamespaceA == #methodMockA.		
		self assert:  mockB callNamespaceA == #methodMockB.		
		self assert:  mockC callNamespaceA == #methodMockB.		
		
		self assert:  mockA callNamespaceB == #methodMockA.
		self assert:  mockB callNamespaceB == #methodMockA.
		self assert:  mockC callNamespaceB == #methodMockC.
		
					 	
	]		
	
						
]